#pragma once

#include "../Texture.hpp"
#include "vml.hpp"

using namespace vml;

struct Vertex {
    vec3 Position;		//v
    vec3 Normal;		//vn
    vec2 TexCoords;	//vt
	int triID;
};

struct Material {
    std::string name;
    vec3 ambient{1.0f};
    vec3 diffuse{1.0f};
    vec3 specular{1.0f};
    float shininess = 32.0f;
    float opacity = 1.0f;

    std::string mapKdPath; // diffuse texture file path
    std::string mapKsPath; // specular texture
    std::string mapBumpPath; // normal/bump map

    Texture diffuseTex;
    Texture specularTex;
    Texture normalTex;
};

struct Setup {

	bool applyCustomTexture = false;
	bool showFaces = false;
	bool showLines = false;
	bool showColors = false;
	bool showPoints = false;

	Texture custom;

	//Light params
	vec3 lightPos{0., 0., 1.};
	vec3 lightColor{1.0f};
	vec3 viewPos{0., 0., 1.};

	float scaleFactor = 1;
};