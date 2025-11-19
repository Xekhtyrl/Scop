#include "Includes/header.h"
#include "Includes/vml.hpp"

void rotationKey(GLFWwindow *window){
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS){
		model = 
			translation(center * -1) *
			rotation(radians(5), vec3{0,1,0}) *
			translation(center) *
			model;
	}
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS){
		model = 
			translation(center * -1) *
			rotation(radians(-5), vec3{0,1,0}) *
			translation(center) *
			model;
	}
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS){
		model = 
			translation(center * -1) *
			rotation(radians(5), vec3{1,0,0}) *
			translation(center) *
			model;
	}
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS){
		model = 
			translation(center * -1) *
			rotation(radians(-5), vec3{1,0,0}) *
			translation(center) *
			model;
	}
}

void translationKey(GLFWwindow *window) {
	if (glfwGetKey(window, GLFW_KEY_KP_4) == GLFW_PRESS){
		center[0] += 0.05;
		model[0][3] -= 0.05;
	}
	if (glfwGetKey(window, GLFW_KEY_KP_6) == GLFW_PRESS){
		center[0] -= 0.05;
		model[0][3] += 0.05;
	}
	if (glfwGetKey(window, GLFW_KEY_KP_8) == GLFW_PRESS){
		center[1] -= 0.05;
		model[1][3] += 0.05;
	}
	if (glfwGetKey(window, GLFW_KEY_KP_2) == GLFW_PRESS){
		center[1] += 0.05;
		model[1][3] -= 0.05;
	}
	if (glfwGetKey(window, GLFW_KEY_KP_1) == GLFW_PRESS){
		center[2] -= 0.05;
		model[2][3] += 0.05;
	}
	if (glfwGetKey(window, GLFW_KEY_KP_9) == GLFW_PRESS){
		center[2] += 0.05;
		model[2][3] -= 0.05;
	}
}

void scaleAndResetKey(GLFWwindow *window) {
	if (glfwGetKey(window, GLFW_KEY_KP_SUBTRACT) == GLFW_PRESS){
		model *= scale(vec3{0.9,0.9,0.9});
		setup.scaleFactor *= 0.9;
	}
	if (glfwGetKey(window, GLFW_KEY_KP_ADD) == GLFW_PRESS){
		model *= scale(vec3{1.1,1.1,1.1});
		setup.scaleFactor *= 1.1;
	}
	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS){
		setBaseModelMatrix(window);
	}
}

void changeSetup(GLFWwindow *window, int key, int action) {
	if (key == GLFW_KEY_F && action == GLFW_RELEASE){
		setup.showFaces = !setup.showFaces;
		setup.showLines = false;
		setup.showColors = false;
		setup.showPoints = false;
		setup.applyCustomTexture = false;
	}
	if (key == GLFW_KEY_L && action == GLFW_RELEASE){
		setup.showLines = !setup.showLines;
		setup.showFaces = false;
		setup.showColors = false;
		setup.showPoints = false;
		setup.applyCustomTexture = false;
	}
	if (key == GLFW_KEY_T && action == GLFW_RELEASE){
		setup.applyCustomTexture = !setup.applyCustomTexture;
		setup.showLines = false;
		setup.showFaces = false;
		setup.showColors = false;
		setup.showPoints = false;
	}
	if (key == GLFW_KEY_C && action == GLFW_RELEASE){
		setup.showColors = !setup.showColors;
		setup.showLines = false;
		setup.showFaces = false;
		setup.showPoints = false;
		setup.applyCustomTexture = false;
	}
	if (key == GLFW_KEY_P && action == GLFW_RELEASE){
		setup.showPoints = !setup.showPoints;
		setup.showLines = false;
		setup.showFaces = false;
		setup.showColors = false;
		setup.applyCustomTexture = false;
	}
}

void changeLightSettings(GLFWwindow *window) {
	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS){
		setup.lightPos[0] += 0.01;
		if (setup.lightPos[0] >= 1)
			setup.lightPos[0] = -1.;
	}
	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS){
		setup.lightPos[1] += 0.01;
		if (setup.lightPos[1] >= 1)
			setup.lightPos[1] = -1.;
	}
	if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS){
		setup.lightPos[2] += 0.01;
		if (setup.lightPos[2] >= 1)
			setup.lightPos[2] = -1.;
	}
	if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS){
		setup.lightColor[0] += 0.01;
		if (setup.lightColor[0] >= 1)
			setup.lightColor[0] = 0.;
	}
	if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS){
		setup.lightColor[1] += 0.01;
		if (setup.lightColor[1] >= 1)
			setup.lightColor[1] = 0.;
	}
	if (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS){
		setup.lightColor[2] += 0.01;
		if (setup.lightColor[2] >= 1)
			setup.lightColor[2] = 0.;
	}
	if (glfwGetKey(window, GLFW_KEY_7) == GLFW_PRESS){
		setup.viewPos[0] += 0.01;
		if (setup.viewPos[0] >= 1)
			setup.viewPos[0] = -1.;
	}
	if (glfwGetKey(window, GLFW_KEY_8) == GLFW_PRESS){
		setup.viewPos[1] += 0.01;
		if (setup.viewPos[1] >= 1)
			setup.viewPos[1] = -1.;
	}
	if (glfwGetKey(window, GLFW_KEY_9) == GLFW_PRESS){
		setup.viewPos[2] += 0.01;
		if (setup.viewPos[2] >= 1)
			setup.viewPos[2] = -1.;
	}
	if (glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS){
		setup.lightPos = vec3{0., 0., 1.};
		setup.lightColor = vec3{1.};
		setup.viewPos = vec3{0., 0., 1.};
	}
}