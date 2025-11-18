#pragma once

#include "../Texture.hpp"
#include "vml.hpp"

using namespace vml;

struct Vertex {
    vec3 Position;		//v
    vec3 Normal;		//vn
    vec2 TexCoords;	//vt
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