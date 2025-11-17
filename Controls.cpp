#include "Includes/header.h"
#include "Includes/vml.hpp"

void rotationKey(GLFWwindow *window){
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS){
		std::cout << "rotate left" <<std::endl;
		model *= rotation(radians(5), vec3{0,2,0});
	}
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS){
		std::cout << "rotate right" <<std::endl;
		model *= rotation(radians(-5), vec3{0,2,0});
	}
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS){
		std::cout << "rotate up" <<std::endl;
		model *= rotation(radians(5), vec3{1,0,0});
	}
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS){
		std::cout << "rotate down" <<std::endl;
		model *= rotation(radians(-5), vec3{1,0,0});
	}
}

void translationKey(GLFWwindow *window) {
	if (glfwGetKey(window, GLFW_KEY_KP_4) == GLFW_PRESS)
		model[0][3] -= 0.05;
	if (glfwGetKey(window, GLFW_KEY_KP_6) == GLFW_PRESS)
		model[0][3] += 0.05;
	if (glfwGetKey(window, GLFW_KEY_KP_8) == GLFW_PRESS)
		model[1][3] += 0.05;
	if (glfwGetKey(window, GLFW_KEY_KP_2) == GLFW_PRESS)
		model[1][3] -= 0.05;
	if (glfwGetKey(window, GLFW_KEY_KP_1) == GLFW_PRESS)
		model[2][3] += 0.05;
	if (glfwGetKey(window, GLFW_KEY_KP_9) == GLFW_PRESS)
		model[2][3] -= 0.05;
}

void scaleAndResetKey(GLFWwindow *window) {
	if (glfwGetKey(window, GLFW_KEY_KP_SUBTRACT) == GLFW_PRESS){
		std::cout << "scale down" <<std::endl;
		model.print();
		model *= scale(vec3{0.9,0.9,0.9});
		model.print();
	}
	if (glfwGetKey(window, GLFW_KEY_KP_ADD) == GLFW_PRESS){
		std::cout << "scale up" <<std::endl;
		model *= scale(vec3{1.1,1.1,1.1});
	}
	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS){
		setBaseModelMatrix();
	}
}