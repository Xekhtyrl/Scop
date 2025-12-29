#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <GL/glext.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <vml.hpp>

//utils.cpp
void strTrim(std::string& str, std::string arr = " \t\r\n");
std::string fileToStr(const std::string& filePath);


#include "../Texture.hpp"
#include "struct.hpp"
#include "../Shader.hpp"

extern Setup setup;
extern const unsigned int SCR_WIDTH;
extern const unsigned int SCR_HEIGHT;
extern float lastX;
extern float lastY;
extern float deltaTime;

#include "../Camera.hpp"
extern Camera camera;
#include "../Mesh.hpp"
#include "../Model.hpp"

class Model;

extern vml::mat4 model;
extern vml::vec3 center;

// #include <globals.hpp>


#include "../Shader.hpp"
#include "../Camera.hpp"
//modelMatrices.cpp
void setBaseModelMatrix(GLFWwindow *window, Model& object);
void defineMatrices(Shader& shad);

//controls.cpp
void scaleAndResetKey(GLFWwindow *window, Model& object);
void rotationKey(GLFWwindow *window);
void translationKey(GLFWwindow *window);
void changeSetup(GLFWwindow *window, int key, int action);
void changeLightSettings(GLFWwindow *window);
void processInput(GLFWwindow *window, Model& object);
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void setup_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

//window.cpp
GLFWwindow* initWindow(std::string name);
void initImgui(GLFWwindow* window);
void createUIImgui();
