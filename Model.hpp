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

class Mesh;

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

class Model 
{
	public:
		//constructors and destructors
		Model();
		Model(char *path);
		Model& operator=(const Model& oth);
		~Model();

		// call function to draw each meshes in model
		void Draw(Shader &shader);

		void printMeshNames();
		//getters
		size_t ms();
		std::vector<Mesh> getMeshes();
		vec3 min();
		vec3 max();
	private:
		// model data
		std::vector<Mesh> meshes;
		std::unordered_map<std::string, Material> materials;
		std::string directory;
		std::string _name;
		vec3 _min = { +MAXFLOAT, +MAXFLOAT, +MAXFLOAT };
		vec3 _max = { -MAXFLOAT, -MAXFLOAT, -MAXFLOAT };
		bool final = false;

		void	loadMtl(std::string path);
		
		//loader utils
		void	defineMinMax(float x, float y, float z);
		bool	validObjPath(std::string path);
		void	convertMtlPath(std::string& mtlpath);

		//in ModelLoadObj.cpp
		void	loadModel(std::string path);
		
		//loadObj sub functions
		int		faceLineParse(std::stringstream& ss, std::vector<vec3>& temp_v, std::vector<vec2>& temp_vt,
					std::vector<vec3>& temp_vn, Mesh& currentMesh, std::unordered_map<VertexKey, unsigned int, VertexKeyHash>& cache);
		void	usemtl(std::stringstream& ss, Mesh& currentMesh, std::string& prevMat, std::unordered_map<VertexKey, unsigned int, VertexKeyHash>& cache);
		void	finishAndResetMesh(Mesh& currentMesh, std::string prevMat, std::unordered_map<VertexKey, unsigned int, VertexKeyHash>& cache, bool reset);
};
