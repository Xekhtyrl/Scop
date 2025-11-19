// globals.hpp
#ifndef GLOBALS_HPP
#define GLOBALS_HPP
#include "vml.hpp"
#include "struct.hpp"

extern Setup setup;
extern const unsigned int SCR_WIDTH;
extern const unsigned int SCR_HEIGHT;
extern float lastX;
extern float lastY;

#include "../Model.hpp"
extern Model object;
extern vml::mat4 model;
extern vml::vec3 center;

#endif
