#include "Model.hpp"


/// @brief default constructor
Model::Model() {}

/// @brief custom cronstructor that load an object into the Model and devide them in meshes and materials
/// @param path argument given to the program as the path the .obj
/// @throw any exception caught by the loadModel function
Model::Model(char *path)
{
	try {
		loadModel(path);
	}
	catch(std::exception &e) {
		throw;
	}
}

/// @brief copy assignement overload constructor
/// @param oth Model to copy
/// @return new model
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

/// @brief Model destructor: destroy and clean all thing related to the Model (Textures, Mehes's VAO, VBO, EBO)
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

/// @brief Model Draw function that call each Mesh Draw function with the shader program needed for it
/// @param shader shader program class
void Model::Draw(Shader &shader) {
	for (Mesh& x : meshes)
		x.Draw(shader, materials[x.materialName()]);
}

//getters
/// @brief debug function to get each Mesh's Material Name
void Model::printMeshMatNames() {
	for (auto& x : meshes)
		std::cout << x.materialName() <<std::endl;
}

/// @brief debug function to get the number of Meshes in the Model
/// @return number of Mesh in Model
size_t Model::ms() {return meshes.size();}
std::vector<Mesh> Model::getMeshes() {return meshes;}
vec3 Model::min() {return _min;}
vec3 Model::max() {return _max;}


/// @brief check new values and (re)define min and max value if needed 
/// @param x x value to compare with previous values
/// @param y y value to compare with previous values
/// @param z z value to compare with previous values
void Model::defineMinMax(float x, float y, float z) {
	_min[0] = std::min(_min[0], x);
	_min[1] = std::min(_min[1], y);
	_min[2] = std::min(_min[2], z);

	_max[0] = std::max(_max[0], x);
	_max[1] = std::max(_max[1], y);
	_max[2] = std::max(_max[2], z);
}

/// @brief function called by loadModel when mtllib is found in the .obj to create all Materials needed and stock them in the materials map
/// @param path to the .mtl file
/// @throw an exception if the file could not be opened
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

/// @brief utilitary function  that check if the file is a .obj and is longer that 4 (no ".obj" file only)
/// @param path the .obj file path
/// @return true if valid file
bool Model::validObjPath(std::string path) {
	if (path.find(".obj", path.size() - 5) > path.size()){
		return false;
	}
	if (path.size() < 5){
		return false;
	}
	return true;
}

/// @brief utilitary function that parse the mtl file to fit
/// @param mtlpath .mtl file path given by the .obj
void Model::convertMtlPath(std::string& mtlpath) {
	if (mtlpath[0] == '.')
		strTrim(mtlpath, ".");
	if (mtlpath[0] != '/')
		mtlpath.insert(mtlpath.begin(), '/');
}
