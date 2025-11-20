#include "Model.hpp"

static void parseFaceVertex(const std::string& token, int &vId, int &vtId, int &vnId)
{
	vId = vtId = vnId = 0;
	size_t p1 = token.find('/');
	if (p1 == std::string::npos) {
		vId = std::stoi(token);
		return;
	}
	size_t p2 = token.find('/', p1 + 1);
	if (p2 == std::string::npos) {
		vId = std::stoi(token.substr(0, p1));
		std::string a = token.substr(p1 + 1);
		if (!a.empty()) vtId = std::stoi(a);
		return;
	}
	vId = std::stoi(token.substr(0, p1));
	if (p2 == p1 + 1) {
		std::string b = token.substr(p2 + 1);
		if (!b.empty()) vnId = std::stoi(b);
	} else {
		vtId = std::stoi(token.substr(p1 + 1, p2 - p1 - 1));
		std::string c = token.substr(p2 + 1);
		if (!c.empty()) vnId = std::stoi(c);
	}
}

inline int objIndexToZeroBased(int id, size_t size) {
	if (id > 0) return id - 1;
	if (id < 0) return static_cast<int>(size) + id;
	return -1;
}

void Model::finishAndResetMesh(Mesh& currentMesh, std::string prevMat, std::unordered_map<VertexKey, unsigned int, VertexKeyHash>& cache, bool reset) {
	if (!currentMesh.vertices().empty()) {
		if (currentMesh.materialName().empty()) currentMesh.materialName(prevMat);
		currentMesh.setupMesh(_min, _max - _min);
		meshes.push_back(currentMesh);
		if (reset){
			currentMesh = Mesh();
			cache.clear();
		}
	}
}

void Model::usemtl(std::stringstream& ss, Mesh& currentMesh, std::string& prevMat, std::unordered_map<VertexKey, unsigned int, VertexKeyHash>& cache) {
	finishAndResetMesh(currentMesh, prevMat, cache, true);
	std::string matName;
	ss >> matName;
	if (matName.find_last_of(":") < matName.size())
		matName = matName.substr(matName.find_last_of(":") + 1);
	if (materials.count(matName) == 0) {
		throw std::runtime_error("Error: Material not found in .mtl file: " + matName);
	}
	currentMesh.materialName(matName);
	prevMat = matName;
}

int Model::faceLineParse(std::stringstream& ss, std::vector<vec3>& temp_v, std::vector<vec2>& temp_vt,
	std::vector<vec3>& temp_vn, Mesh& currentMesh, std::unordered_map<VertexKey, unsigned int, VertexKeyHash>& cache) {
	// collect face tokens, convert to indices (with dedup)
	std::vector<unsigned int> faceIndices;
	std::string token;
	while (ss >> token) {
		int vId=0, vtId=0, vnId=0;
		parseFaceVertex(token, vId, vtId, vnId);

		// Map negative indices to 0-based
		int vIndex  = objIndexToZeroBased(vId,  temp_v.size());
		int vtIndex = objIndexToZeroBased(vtId, temp_vt.size());
		int vnIndex = objIndexToZeroBased(vnId, temp_vn.size());

		if (vIndex < 0 || vIndex >= static_cast<int>(temp_v.size()))
			throw std::runtime_error("OBJ parse error: vertex index out of range");

		VertexKey key { vIndex, vtIndex, vnIndex };

		unsigned int finalIndex;
		auto it = cache.find(key);
		if (it != cache.end()) {
			finalIndex = it->second;
		} else {
			// create new vertex
			Vertex vert;
			vert.Position = temp_v[vIndex];
			if (vtIndex >= 0) vert.TexCoords = temp_vt[vtIndex];
			else vert.TexCoords = vec2{0.0f, 0.0f};
			if (vnIndex >= 0) vert.Normal = temp_vn[vnIndex];
			else vert.Normal = vec3{0.0f, 0.0f};

			currentMesh.vertices().push_back(vert);
			finalIndex = static_cast<unsigned int>(currentMesh.vertices().size() - 1);
			cache.emplace(key, finalIndex);
		}
		faceIndices.push_back(finalIndex);
	}

	// triangulate the face using fan (0,i,i+1)
	if (faceIndices.size() < 3) {
		// ignore lines with less than 3 vertices
		return 0;
	}
	for (size_t i = 1; i + 1 < faceIndices.size(); ++i) {
		currentMesh.indices().push_back(faceIndices[0]);
		currentMesh.indices().push_back(faceIndices[i]);
		currentMesh.indices().push_back(faceIndices[i+1]);
	}
	return 1;
}

void Model::loadModel(std::string path) {
	if (!validObjPath(path))
		throw std::runtime_error("Error: Invalid file name/extension.");

	std::ifstream file(path);
	if (!file.is_open())
		throw std::runtime_error("Error: Object File could not be opened or does not exist.");
	
	std::vector<vec3> temp_v;
	std::vector<vec3> temp_vn;
	std::vector<vec2> temp_vt;
	std::string prevMat;
	float x, y, z;
	Mesh currentMesh;

	directory = path.substr(0, path.find_last_of("/"));
	meshes.clear();
	materials.clear();

	std::string line;

	std::unordered_map<VertexKey, unsigned int, VertexKeyHash> cache;

	while (std::getline(file, line)) {
		if (line.empty()) continue;
		std::stringstream ss(line);
		std::string type;
		ss >> type;
		if (type.empty() || type[0] == '#') continue;

		if (type == "v") {
			ss >> x >> y >> z;
			temp_v.push_back({x,y,z});
			defineMinMax(x, y, z);
		}
		else if (type == "vt") {
			ss >> x >> y;
			temp_vt.push_back({x,y});
			currentMesh.vtPresent(true);
		}
		else if (type == "vn") {
			currentMesh.vnPresent(true);
			ss >> x >> y >> z;
			temp_vn.push_back({x,y,z});
		}
		else if (type == "f") {
			try {
				if (!faceLineParse(ss, temp_v, temp_vt, temp_vn, currentMesh, cache))
					continue;
			}
			catch (std::exception&e) {
				throw;
			}
		}
		else if (type == "g") {
			finishAndResetMesh(currentMesh, prevMat, cache, true);
			std::string name;
			ss >> name;
			currentMesh.name(name);
		}
		else if (type == "o") {
			ss >> _name;
		}
		else if (type == "usemtl") {
			usemtl(ss, currentMesh, prevMat, cache);
		}
		else if (type == "mtllib") {
			std::string mtlpath;
			ss >> mtlpath;
			convertMtlPath(mtlpath);
			loadMtl(mtlpath);
		}
	}

	finishAndResetMesh(currentMesh, prevMat, cache, false);

	file.close();
}