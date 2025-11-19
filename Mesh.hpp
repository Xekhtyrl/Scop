#pragma once
#include <vector>
#include <iostream>
#include <algorithm>

#include "Shader.hpp"
#include "Includes/vml.hpp"
#include "Includes/struct.hpp"
#include <header.h>

class Mesh {
    public:
        // mesh data
        std::string                 _name;
        std::vector<Vertex>			_vertices;
        std::vector<unsigned int>	_indices;
        std::string                 _materialName;
		bool						vnPresent = false;
		bool						vtPresent = false;
		// unsigned int				VAO;

        Mesh() {VAO = VBO = EBO = 0;}
        Mesh& operator=(const Mesh& oth) {
            _vertices = oth._vertices;
            _indices = oth._indices;
            _materialName = oth._materialName;
            return *this;
        }
        Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::string textures);
		
		void Draw(Shader &shader, Material material) {
			shader.use();
			glBindVertexArray(VAO);
			if (setup.showLines){
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				glLineWidth(0.1f);
			}
			else if (setup.showPoints){
				glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
				glLineWidth(1.f);
			}
			else
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

			// custom or diffuse
			if (setup.applyCustomTexture && setup.custom.id()){
				glActiveTexture(GL_TEXTURE0);
				shader.setInt("customTex", 0);
				glBindTexture(GL_TEXTURE_2D,setup.custom.id());
			}
			else if (material.diffuseTex.id() != 0) {
				glActiveTexture(GL_TEXTURE1);
				shader.setInt("material.diffuse", 1);
				glBindTexture(GL_TEXTURE_2D, material.diffuseTex.id());
			}

			// specular
			if (material.specularTex.id() != 0) {
				glActiveTexture(GL_TEXTURE2);
				shader.setInt("material.specular", 2);
				glBindTexture(GL_TEXTURE_2D, material.specularTex.id());
			}

			// normal
			if (material.normalTex.id() != 0) {
				glActiveTexture(GL_TEXTURE3);
				shader.setInt("material.normalMap", 3);
				glBindTexture(GL_TEXTURE_2D, material.normalTex.id());
			}

			// booleans
			shader.setBool("showFaces", setup.showFaces);
			shader.setBool("changeColor", setup.showColors);
			shader.setBool("useCustomTex", setup.applyCustomTexture && setup.custom.id() != 0);
			shader.setBool("useDiffuseMap",  material.diffuseTex.id()  != 0);
			shader.setBool("useSpecularMap", material.specularTex.id() != 0);
			shader.setBool("useNormalMap",   material.normalTex.id()   != 0);

			// scalar uniforms
			shader.setVec3("material.ambient",        material.ambient);
			shader.setVec3("material.diffuseColor",   material.diffuse);
			shader.setVec3("material.specularColor",  material.specular);
			shader.setFloat("material.shininess",     material.shininess);
			shader.setFloat("material.opacity",       material.opacity);

			// light
			shader.setVec3("lightPos", setup.lightPos);
			shader.setVec3("lightColor", setup.lightColor);
			shader.setVec3("viewPos", setup.viewPos);

			glBindVertexArray(VAO);
			// draw
			glDrawElements(GL_TRIANGLES, _indices.size(), GL_UNSIGNED_INT, 0);

			glBindVertexArray(0);
			glActiveTexture(GL_TEXTURE0);
		}

		void setupMesh(vec3 min, vec3 size) {
			if (!vnPresent){
							// Loop through triangles (3 indices per triangle)
				for (size_t i = 0; i < _indices.size(); i += 3) {

					unsigned int i0 = _indices[i];
					unsigned int i1 = _indices[i + 1];
					unsigned int i2 = _indices[i + 2];

					vec3 &p0 = _vertices[i0].Position;
					vec3 &p1 = _vertices[i1].Position;
					vec3 &p2 = _vertices[i2].Position;

					vec3 faceNormal = normalize(cross(p1 - p0, p2 - p0));

					// Accumulate for shading normal
					_vertices[i0].Normal += faceNormal;
					_vertices[i1].Normal += faceNormal;
					_vertices[i2].Normal += faceNormal;

					// Use *faceNormal* for UV projection — this is the fix
					if (!vtPresent){
						_vertices[i0].TexCoords = generateCubicUV(p0, faceNormal, min, size);
						_vertices[i1].TexCoords = generateCubicUV(p1, faceNormal, min, size);
						_vertices[i2].TexCoords = generateCubicUV(p2, faceNormal, min, size);
					}
				}
				vtPresent = true;

				// Normalize final normals
				for (auto &v : _vertices)
					v.Normal = normalize(v.Normal);
			}
			for (int i = 0; i < _indices.size(); i += 3) {
				int triID = i / 3;
				_vertices[ _indices[i] ].triID = triID;
				_vertices[ _indices[i+1] ].triID = triID;
				_vertices[ _indices[i+2] ].triID = triID;
			}
			glGenVertexArrays(1, &VAO);
			glGenBuffers(1, &VBO);
			glGenBuffers(1, &EBO);
		
			glBindVertexArray(VAO);
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			// changed &_vertices[0] to _vertices.data(), same for _indices, just in case. Might change it back
			glBufferData(GL_ARRAY_BUFFER, _vertices.size() * sizeof(Vertex), &_vertices[0], GL_STATIC_DRAW);  

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indices.size() * sizeof(unsigned int), 
						&_indices[0], GL_STATIC_DRAW);

			// vertex positions
			glEnableVertexAttribArray(0);	
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
			// vertex normals
			glEnableVertexAttribArray(1);	
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
			// vertex texture coords
			glEnableVertexAttribArray(2);	
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

			glEnableVertexAttribArray(3);
			glVertexAttribIPointer(3, 1, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, triID));
			glBindVertexArray(0);
		}

		vec2 generateCubicUV(const vec3& p, const vec3& n, 
                     const vec3& min, const vec3& size)
		{
			vec2 uv;

			// Find dominant axis of normal
			float ax = fabs(n[0]);
			float ay = fabs(n[1]);
			float az = fabs(n[2]);

			if (ax > ay && ax > az) {
				// X-dominant → project onto YZ
				uv[0] = (p[2] - min[2]) / size[2];
				uv[1] = (p[1] - min[1]) / size[1];
			}
			else if (ay > ax && ay > az) {
				// Y-dominant → project onto XZ
				uv[0] = (p[0] - min[0]) / size[0];
				uv[1] = (p[2] - min[2]) / size[2];
			}
			else {
				// Z-dominant → project onto XY
				uv[0] = (p[0] - min[0]) / size[0];
				uv[1] = (p[1] - min[1]) / size[1];
			}

			return uv;
		}

		void generateDefaultVT(vec3 min, vec3 max)
		{
			vec3 size = max - min;

			// assign UVs based on X-Y projection
			for (auto& v : _vertices) {
				v.TexCoords = generateCubicUV(v.Position, v.Normal, min, size);
			}
		}


		//getters
        std::vector<Vertex>& vertices() {return _vertices;}
        std::vector<Vertex> vertices() const {return _vertices;}
        std::vector<unsigned int>& indices() {return _indices;}
        std::vector<unsigned int> indices() const {return _indices;}
        std::string materialName() const {return _materialName;}
        std::string name() const {return _name;}

		//setters
        void vertices(std::vector<Vertex>& vertices) {_vertices = vertices;}
        void indices(std::vector<unsigned int>& idxs) {_indices = idxs;}
        void materialName(std::string matName) {_materialName = matName;}
        void name(std::string name) {_name = name;}
		GLuint VAO;
		GLuint VBO;
		GLuint EBO;
    private:
        //  render data
};
