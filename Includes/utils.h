#pragma once

#include <iostream>
#include <glad/glad.h> 
#include <GLFW/glfw3.h>
#include <GL/glext.h>

std::string floatToStr(float value);
void strTrim(std::string& str, std::string arr = " \t\r\n");
std::string fileToStr(std::string filePath);
void rotationKey(GLFWwindow *window);
void translationKey(GLFWwindow *window);
void scaleAndResetKey(GLFWwindow *window);
void setBaseModelMatrix(GLFWwindow *window);
void changeSetup(GLFWwindow *window, int key, int action);
void changeLightSettings(GLFWwindow *window);