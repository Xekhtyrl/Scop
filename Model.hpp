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

#include <tuple>

// A compact key for identifying unique (v/vt/vn) triplets
struct VertexKey {
	unsigned int vId;
	unsigned int vtId;
	unsigned int vnId;

	bool operator==(const VertexKey& other) const {
		return vId == other.vId && vtId == other.vtId && vnId == other.vnId;
	}
};

// Custom hash for unordered_map
namespace std {
	template <>
	struct hash<VertexKey> {
		size_t operator()(const VertexKey& k) const {
			return ((std::hash<unsigned int>()(k.vId) ^
					(std::hash<unsigned int>()(k.vtId) << 1)) >> 1) ^
					(std::hash<unsigned int>()(k.vnId) << 1);
		}
	};
}


static void parseFaceVertex(const std::string& token, unsigned int& vId, unsigned int& vtId, unsigned int& vnId)
{
	vId = vtId = vnId = 0;
	std::string tok = token;
	std::string v, vt, vn;
	
	if (token.find("//") != std::string::npos) {
		// Handle "v//vn" (no texture coordinate)
		std::replace(tok.begin(), tok.end(), '/', ' ');
		std::stringstream ss(tok);
		ss >> v >> vn;
		vId = std::stoi(v);
		vnId = std::stoi(vn);
	} else {
		std::stringstream ss(tok);
		// Handle "v/vt/vn" or "v/vt"
		char slash;
		ss >> vId;
		if (ss.peek() == '/') {
			ss >> slash;
			if (ss.peek() != '/') {
				ss >> vtId;
			}
			if (ss.peek() == '/') {
				ss >> slash;
				ss >> vnId;
			}
		}
	}
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
			}
			return *this;
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

			std::string tmp;
			while (getline(file, tmp)) {
				std::stringstream ss(tmp);
				std::string type;
				ss >> type;

				if (tmp.empty() || type == "#")
					continue;
				if (type == "vt") {
					ss >> x >> y;
					temp_vt.push_back({x, y});
				}
				else if (type == "vn") {
					ss >> x >> y >> z;
					temp_vn.push_back({x,y,z});
				}
				else if (type == "v") {
					ss >> x >> y >> z;
					temp_v.push_back({x,y,z});
					defineMinMax(x, y, z);
				}
				else if (type == "f") {
					std::vector<unsigned int> faceIndices;
					std::string fLine;

					while (ss >> fLine) {
						unsigned int vId = 0, vtId = 0, vnId = 0;
						parseFaceVertex(fLine, vId, vtId, vnId);

						Vertex vert;
						if (vId > 0)  vert.Position = temp_v[vId - 1];
						if (vtId > 0) vert.TexCoords = temp_vt[vtId - 1];
						if (vnId > 0) vert.Normal = temp_vn[vnId - 1];

						currentMesh.vertices().push_back(vert);
						faceIndices.push_back(currentMesh.vertices().size() - 1);
					}

					// Triangulate the face (fan method)
					for (size_t i = 1; i + 1 < faceIndices.size(); i++) {
						currentMesh.indices().push_back(faceIndices[0]);
						currentMesh.indices().push_back(faceIndices[i]);
						currentMesh.indices().push_back(faceIndices[i + 1]);
					}
				}
				else if (tmp.find("g") == 0) {
					if (!currentMesh.vertices().empty()) {
						// std::cout << currentMesh <<std::endl;
						if (currentMesh.materialName().empty())
							currentMesh.materialName(prevMat);
						currentMesh.setupMesh();
						meshes.push_back(currentMesh);
						totalMesh += 1;
						currentMesh = Mesh();
					}
					std::string name;
					ss >> name;
					currentMesh.name(name);

				}
				else if (type == "o")
					ss >> _name;
				else if (type == "usemtl") {
					if (!currentMesh.vertices().empty()) {
						// std::cout << currentMesh <<std::endl;
						meshes.push_back(currentMesh);
						if (currentMesh.materialName().empty())
							currentMesh.materialName(prevMat);
						currentMesh.setupMesh();
						std::string name = currentMesh.name();
						currentMesh = Mesh();
						currentMesh.name(name);
						totalMesh +=1;
					}
					std::string matName;
					ss >> matName;
					if (matName.find_last_of(":") < matName.size())
						matName = matName.substr(matName.find_last_of(":") + 1);
					if (materials.count(matName) == 0){
						throw std::runtime_error("Error: Materia not found in .mtl file: " + matName);
					}
					currentMesh.materialName(matName);
					prevMat = matName;
				}
				else if (type == "mtllib") {
					std::string mtlpath;
					ss >> mtlpath;
					convertMtlPath(mtlpath);
					try {
						loadMtl(mtlpath);
					}
					catch(std::exception& e) {
						throw;
					}
				}
			}
			if (!currentMesh.vertices().empty()){
				// std::cout << currentMesh <<std::endl;
				if (currentMesh.materialName().empty())
					currentMesh.materialName(prevMat);
				currentMesh.setupMesh();
				meshes.push_back(currentMesh);
				totalMesh += 1;
			}
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
				else if (type == "mapKd")
					ss >> currentMaterial.mapKdPath;
				else if (type == "mapKs")
					ss >> currentMaterial.mapKsPath;
				else if (type == "map_Bump" || type == "bump")
					ss >> currentMaterial.mapBumpPath;
			}
			if (!currentMaterial.name.empty())
				materials[currentMaterial.name] = currentMaterial;

			file.close();

			// Load textures
			for (auto& [name, mat] : materials) {
				if (!mat.mapKdPath.empty())
					mat.diffuseTex = Texture(directory + mat.mapKdPath.c_str());
				if (!mat.mapKsPath.empty())
					mat.specularTex = Texture(directory + mat.mapKsPath.c_str());
				if (!mat.mapBumpPath.empty())
					mat.normalTex = Texture(directory + mat.mapBumpPath.c_str());
			}
		}
		// void processNode(aiNode *node, const aiScene *scene);
		// Mesh processMesh(aiMesh *mesh, const aiScene *scene);
		// std::vector<Textures> loadMaterialTextures(aiMaterial *mat, aiTextureType type, 
		//					std::string typeName);
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
