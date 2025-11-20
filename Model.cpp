#include "Model.hpp"

Model::Model() {}

Model::Model(char *path)
{
	try {
		loadModel(path);
	}
	catch(std::exception &e) {
		throw;
	}
}
Model& Model::operator=(const Model& oth) {
	if (this != &oth) {
		meshes = oth.meshes;
		materials = oth.materials;
		directory = oth.directory;
		_name = oth._name;
		_min = oth._min;
		_max = oth._max;
		final = true;
	}
	return *this;
}
Model::~Model() {
	if (!final)
		return ;
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
		if (mesh.VAO())
			glDeleteVertexArrays(1, &(mesh.VAO()));
		if (mesh.VBO())
			glDeleteBuffers(1, &(mesh.VBO()));
		if (mesh.EBO())
			glDeleteBuffers(1, &(mesh.EBO()));
	}
	if (setup.custom.id())
		setup.custom.deleteTex();
}
void Model::Draw(Shader &shader) {
	for (Mesh& x : meshes)
		x.Draw(shader, materials[x.materialName()]);
}

void Model::printMeshNames() {
	for (auto& x : meshes)
		std::cout << x.materialName() <<std::endl;
}

size_t Model::ms() {return meshes.size();}
std::vector<Mesh> Model::getMeshes() {return meshes;}
vec3 Model::min() {return _min;}
vec3 Model::max() {return _max;}

void Model::defineMinMax(float x, float y, float z) {
	_min[0] = std::min(_min[0], x);
	_min[1] = std::min(_min[1], y);
	_min[2] = std::min(_min[2], z);

	_max[0] = std::max(_max[0], x);
	_max[1] = std::max(_max[1], y);
	_max[2] = std::max(_max[2], z);
}

void Model::loadMtl(std::string path) {
	std::ifstream file(directory + path);
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
		else if (type == "map_Kd")
			ss >> currentMaterial.mapKdPath;
		else if (type == "map_Ks")
			ss >> currentMaterial.mapKsPath;
		else if (type == "map_Bump" || type == "bump")
			ss >> currentMaterial.mapBumpPath;
	}
	if (!currentMaterial.name.empty())
		materials[currentMaterial.name] = currentMaterial;

	file.close();

	// Load textures
	for (auto& it : materials) {
		auto& mat  = it.second;
		if (!mat.mapKdPath.empty())
			mat.diffuseTex.loadTexture(directory + "/" + mat.mapKdPath.c_str());
		if (!mat.mapKsPath.empty())
			mat.specularTex.loadTexture(directory + "/" + mat.mapKsPath.c_str());
		if (!mat.mapBumpPath.empty())
			mat.normalTex.loadTexture(directory + "/" + mat.mapBumpPath.c_str());
	}
}

bool Model::validObjPath(std::string path) {
	if (path.find(".obj", path.size() - 5) > path.size()){
		return false;
	}
	if (path.size() < 5){
		return false;
	}
	return true;
}
void Model::convertMtlPath(std::string& mtlpath) {
	if (mtlpath[0] == '.')
		strTrim(mtlpath, ".");
	if (mtlpath[0] != '/')
		mtlpath.insert(mtlpath.begin(), '/');
}
