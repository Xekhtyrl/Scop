#pragma once
#include <vector>
#include <iostream>
#include <algorithm>

#include "Shader.hpp"
#include "Includes/vml.hpp"
#include "Includes/struct.hpp"

class Mesh {
    public:
        // mesh data
        std::string                 _name;
        std::vector<Vertex>			_vertices;
        std::vector<unsigned int>	_indices;
        std::string                 _materialName;
		// unsigned int				VAO;

        Mesh() {VAO = VBO = EBO = 0;}
        Mesh& operator=(const Mesh& oth) {
            _vertices = oth._vertices;
            _indices = oth._indices;
            _materialName = oth._materialName;
            return *this;
        }
        Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::string textures);
        
		// ~Mesh() {
		// 	if (VAO)
		// 		glDeleteVertexArrays(1, &VAO);
		// 	if (VBO)
		// 		glDeleteBuffers(1, &VBO);
		// 	if (EBO)
		// 		glDeleteBuffers(1, &EBO);
		// }
		
		void Draw(Shader &shader, Material material) {
			unsigned int maxIndex = *std::max_element(_indices.begin(), _indices.end());
			// std::cout << "mesh drawn" << std::endl;
			// Bind textures if they exist
			glBindVertexArray(VAO);
			if (material.diffuseTex.id() != 0) {
				std::cout << "diffuse tex" << std::endl;
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, material.diffuseTex.id());
				shader.setInt("material.diffuse", 0);
			}
			
			if (material.normalTex.id() != 0) {
				std::cout << "normal" << std::endl;
				glActiveTexture(GL_TEXTURE2);
				glBindTexture(GL_TEXTURE_2D, material.normalTex.id());
				shader.setInt("material.normalMap", 1);
			}

			if (material.specularTex.id() != 0) {
				std::cout << "specular" << std::endl;
				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, material.specularTex.id());
				shader.setInt("material.specular", 2);
			}

			shader.setBool("useDiffuseMap", material.diffuseTex.id() != 0);
			shader.setBool("useSpecularMap", material.specularTex.id() != 0);
			shader.setBool("useNormalMap", material.normalTex.id() != 0);
			// Upload scalar/vec3 uniforms
			shader.setVec3("material.ambient", material.ambient);
			shader.setVec3("material.diffuseColor", material.diffuse);
			shader.setVec3("material.specularColor", material.specular);
			shader.setFloat("material.shininess", material.shininess);
			shader.setFloat("material.opacity", material.opacity);

			//set light
			shader.setVec3("lightPos", vec3{1.,1.,1.});
			shader.setVec3("lightColor", vec3{1,1,1});
			shader.setVec3("viewPos", vec3{1,1,1});

			// Draw the mesh
			// glBindVertexArray(VAO);


			glDrawElements(GL_TRIANGLES, _indices.size(), GL_UNSIGNED_INT, 0);
			// GLenum err;
			// while ((err = glGetError()) != GL_NO_ERROR)
			// 	std::cerr << "GL error: " << err << std::endl;
			// std::cerr << "VAO=" << VAO 
			// 	<< " VBO=" << VBO 
			// 	<< " EBO=" << EBO 
			// 	<< " vertices=" << _vertices.size() 
			// 	<< " indices=" << _indices.size() << std::endl;
			glBindVertexArray(0);

			// Reset active texture (optional)
			glActiveTexture(GL_TEXTURE0);
		}

		void setupMesh() {
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

			glBindVertexArray(0);
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
