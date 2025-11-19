#pragma once
#include <vector>
#include <iostream>
#include "header.h"
#include "Shader.hpp"
#include "Mesh.hpp"
#include "Includes/vml.hpp"
#include "Includes/struct.hpp"
#include <unordered_map>
#include <limits>
#include <algorithm>


using namespace vml;


struct VertexKey {
    int v, vt, vn;
    bool operator==(VertexKey const& o) const noexcept {
        return v == o.v && vt == o.vt && vn == o.vn;
    }
};
struct VertexKeyHash {
    size_t operator()(VertexKey const& k) const noexcept {
        return static_cast<size_t>(
            (k.v * 73856093) ^ (k.vt * 19349663) ^ (k.vn * 83492791)
        );
    }
};

static void parseFaceVertex(const std::string& token, int &vId, int &vtId, int &vnId)
{
    vId = vtId = vnId = 0;
    size_t p1 = token.find('/');
    if (p1 == std::string::npos) {
        // "v"
        vId = std::stoi(token);
        return;
    }
    size_t p2 = token.find('/', p1 + 1);
    if (p2 == std::string::npos) {
        // "v/vt"
        vId = std::stoi(token.substr(0, p1));
        std::string a = token.substr(p1 + 1);
        if (!a.empty()) vtId = std::stoi(a);
        return;
    }
    // p2 exists
    vId = std::stoi(token.substr(0, p1));
    if (p2 == p1 + 1) {
        // "v//vn"
        std::string b = token.substr(p2 + 1);
        if (!b.empty()) vnId = std::stoi(b);
    } else {
        // "v/vt/vn"
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

class Model 
{
	public:
		Model() {totalMesh = 0;};
		Model(char *path)
		{
			try {
				totalMesh = 0;
				loadModel(path);
			}
			catch(std::exception &e) {
				throw;
			}
		}
		Model& operator=(const Model& oth) {
			if (this != &oth) {
				meshes = oth.meshes;
				materials = oth.materials;
				directory = oth.directory;
				_name = oth._name;
				totalMesh = oth.totalMesh;
				_min = oth._min;
				_max = oth._max;
				final = true;
			}
			return *this;
		}
		~Model() {
			if (!final)
				return ;
				std::cout << "Model destroyer called" <<std::endl;
			for (auto& it: materials){
				auto& mat = it.second;
				if (mat.diffuseTex.id() != 0)
					mat.diffuseTex.deleteTex();
				if (mat.specularTex.id() != 0)
					mat.specularTex.deleteTex();
				if (mat.normalTex.id() != 0)
					mat.normalTex.deleteTex();
			}
			for (auto& mesh: meshes) {
				if (mesh.VAO)
					glDeleteVertexArrays(1, &(mesh.VAO));
				if (mesh.VBO)
					glDeleteBuffers(1, &(mesh.VBO));
				if (mesh.EBO)
					glDeleteBuffers(1, &(mesh.EBO));
			}
			if (setup.custom.id())
				setup.custom.deleteTex();
		}
		void Draw(Shader &shader) {
			for (Mesh& x : meshes)
				x.Draw(shader, materials[x.materialName()]);
		}

		void printMeshNames() {
			for (auto& x : meshes)
				std::cout << x.materialName() <<std::endl;
		}
		int totalMesh;
		size_t ms() {return meshes.size();}
		std::vector<Mesh> const getMeshes() const {return meshes;}
		vec3 min() {return _min;}
		vec3 max() {return _max;}
	private:
		// model data
		std::vector<Mesh> meshes;
		std::unordered_map<std::string, Material> materials;
		std::string directory;
		std::string _name;
		vec3 _min = { +MAXFLOAT, +MAXFLOAT, +MAXFLOAT };
		vec3 _max = { -MAXFLOAT, -MAXFLOAT, -MAXFLOAT };
		bool final = false;

		void defineMinMax(float x, float y, float z) {
			_min[0] = std::min(_min[0], x);
			_min[1] = std::min(_min[1], y);
			_min[2] = std::min(_min[2], z);

			_max[0] = std::max(_max[0], x);
			_max[1] = std::max(_max[1], y);
			_max[2] = std::max(_max[2], z);
		}

		void loadModel(std::string path) {
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
			// per-mesh cache to deduplicate vertex triples -> vertex index
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
					currentMesh.vtPresent = true;
				}
				else if (type == "vn") {
					currentMesh.vnPresent = true;
					ss >> x >> y >> z;
					temp_vn.push_back({x,y,z});
				}
				else if (type == "f") {
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
						continue;
					}
					for (size_t i = 1; i + 1 < faceIndices.size(); ++i) {
						currentMesh.indices().push_back(faceIndices[0]);
						currentMesh.indices().push_back(faceIndices[i]);
						currentMesh.indices().push_back(faceIndices[i+1]);
					}
				}
				else if (type == "g") {
					// new group: push previous mesh if it has geometry
					if (!currentMesh.vertices().empty()) {
						if (currentMesh.materialName().empty()) currentMesh.materialName(prevMat);
						if (!currentMesh.vtPresent) currentMesh.generateDefaultVT(_min, _max);
						currentMesh.setupMesh(_min, _max - _min);
						meshes.push_back(currentMesh);
						totalMesh++;
						currentMesh = Mesh();
						cache.clear();
					}
					std::string name;
					ss >> name;
					currentMesh.name(name);
				}
				else if (type == "o") {
					ss >> _name;
				}
				else if (type == "usemtl") {
					// when material changes: finalize previous mesh
					if (!currentMesh.vertices().empty()) {
						if (currentMesh.materialName().empty()) {
							currentMesh.materialName(prevMat);
						}
						if (!currentMesh.vtPresent) currentMesh.generateDefaultVT(_min, _max);
						currentMesh.setupMesh(_min, _max - _min);
						meshes.push_back(currentMesh);
						totalMesh++;
						currentMesh = Mesh();
						cache.clear();
					}
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
				else if (type == "mtllib") {
					std::string mtlpath;
					ss >> mtlpath;
					convertMtlPath(mtlpath);
					loadMtl(mtlpath);
				}
				// other tokens ignored
			}

			// push last mesh if exists
			if (!currentMesh.vertices().empty()) {
				if (currentMesh.materialName().empty()) currentMesh.materialName(prevMat);
				if (!currentMesh.vtPresent) currentMesh.generateDefaultVT(_min, _max);
				currentMesh.setupMesh(_min, _max - _min);
				meshes.push_back(currentMesh);
				totalMesh++;
			}

			file.close();
		}

		void loadMtl(std::string path) {
			std::ifstream file(directory + path);
			std::cout << directory + path << std::endl;
			if (!file.is_open())
				throw std::runtime_error("Error: Material File could not be opened or does not exist.");
			std::string line;
			Material currentMaterial;
			float x,y,z;

			while (getline(file, line)) {
				std::stringstream ss(line);
				std::string type;
				ss >> type;

				if (type == "newmtl"){
					if (!currentMaterial.name.empty())
						materials[currentMaterial.name] = currentMaterial; // store previous
					currentMaterial = Material(); // reset
					ss >> currentMaterial.name;
					std::cout << "Material name: " + currentMaterial.name <<std::endl;
				}
				else if (type == "Ka"){
					ss >> x >> y >> z;
					currentMaterial.ambient = vec3{x, y, z};
				}
				else if (type == "Kd"){
					ss >> x >> y >> z;
					currentMaterial.diffuse = vec3{x, y, z};
				}
				else if (type == "Ks"){
					ss >> x >> y >> z;
					currentMaterial.specular = vec3{x, y, z};
				}
				else if (type == "Ns")
					ss >> currentMaterial.shininess;
				else if (type == "d" || type == "Tr")
					ss >> currentMaterial.opacity;
				else if (type == "map_Kd"){
					ss >> currentMaterial.mapKdPath;
					std::cout << currentMaterial.mapKdPath << std::endl;
				}
				else if (type == "map_Ks"){
					ss >> currentMaterial.mapKsPath;
					std::cout << currentMaterial.mapKsPath << std::endl;
				}
				else if (type == "map_Bump" || type == "bump"){
					ss >> currentMaterial.mapBumpPath;
					std::cout << currentMaterial.mapBumpPath << std::endl;
				}
			}
			if (!currentMaterial.name.empty())
				materials[currentMaterial.name] = currentMaterial;

			file.close();

			// Load textures
			for (auto& it : materials) {
				auto& name = it.first;
				auto& mat  = it.second;
				if (!mat.mapKdPath.empty())
					mat.diffuseTex.loadTexture(directory + "/" + mat.mapKdPath.c_str());
				if (!mat.mapKsPath.empty())
					mat.specularTex.loadTexture(directory + "/" + mat.mapKsPath.c_str());
				if (!mat.mapBumpPath.empty())
					mat.normalTex.loadTexture(directory + "/" + mat.mapBumpPath.c_str());
				std::cout
				<< "Material : " << mat.name << "\n"
				<< "\tKa: "; mat.ambient.print();
				std::cout << "\tKd: "; mat.diffuse.print();
				std::cout << "\tKs: "; mat.specular.print();
				std::cout << "\tNs: " << mat.shininess << "\n"
				<< "\td:" << mat.opacity << "\n"
				<< "\tmap_Kd:" << mat.mapKdPath << "\n"
				<< "\tmap_Ks:" << mat.mapKsPath << "\n"
				<< "\tmap_Bump:" << mat.mapBumpPath << "\n"
				<< "\tdifTex:" << mat.diffuseTex.id() << "\n"
				<< "\tspecTex:" << mat.specularTex.id() << "\n"
				<< "\tnormTex:" << mat.normalTex.id() << "\n"
				<< std::endl;
			}
		}

		bool validObjPath(std::string path) {
			if (path.find(".obj", path.size() - 5) > path.size()){
				return false;
			}
			if (path.size() < 5){
				return false;
			}
			return true;
		}
		void convertMtlPath(std::string& mtlpath) {
			if (mtlpath[0] == '.')
				strTrim(mtlpath, ".");
			if (mtlpath[0] != '/')
				mtlpath.insert(mtlpath.begin(), '/');
		}
};
