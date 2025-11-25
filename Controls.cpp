#include "Includes/header.h"
#include "Includes/vml.hpp"


/**
 * @brief main function that regroup and process all inputs (functions)
 * @param window glfw window pointer
 */
void processInput(GLFWwindow *window)
{

	if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
	rotationKey(window);
	translationKey(window);
	scaleAndResetKey(window);
	changeLightSettings(window);
}

/**
 * @brief function that catch the mouse mouvements input and set the camera in accordance
 * @param window glfw window pointer
 * @param xposIn x postion of the mouse
 * @param yposIn y postion of the mouse
 */
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
	float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);
	
	if (camera.firstMouse)
	{
		glfwSetCursorPos(window, SCR_WIDTH / 2.0, SCR_HEIGHT / 2.0);
		lastX = xpos;
		lastY = ypos;
		camera.firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

/**
 * @brief function that catch the mouse scroll mouvements input and set the camera in accordance (zoom)
 * @param window glfw window pointer
 * @param xposIn x postion of the mouse
 * @param yposIn y postion of the mouse
 */
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	(void)window;
	(void)xoffset;
	camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

/**
 * @brief set the key input for the setups values such as the differents viewmode of the model
 * @param window glfw window pointer (glfwSetKeyCallback required params)
 * @param key key pressed id (glfwSetKeyCallback required params)
 * @param scancode unused (glfwSetKeyCallback required params)
 * @param action action linked to the key (glfwSetKeyCallback required params)
 * @param mods unused (glfwSetKeyCallback required params)
 */
void setup_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	(void)scancode;
	(void)mods;
	changeSetup(window, key, action);
}

/**
 * @brief input linked to rotation of model (the translation is needed for rotation around center axis)
 * @param window glfw window pointer
 */
void rotationKey(GLFWwindow *window){
	bool ctrlDown = glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS
             || glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS;
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
	if (!ctrlDown){
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
	}
	else {
		if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS){
			model = 
			translation(center * -1) *
			rotation(radians(5), vec3{0,0,1}) *
			translation(center) *
			model;
		}
		if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS){
			model = 
			translation(center * -1) *
			rotation(radians(-5), vec3{0,0,1}) *
			translation(center) *
			model;
		}
	}
}

/**
 * @brief input linked to translation of model
 * @param window glfw window pointer
 */
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

/**
 * @brief input linked to scale of model and reset
 * @param window glfw window pointer
 */
void scaleAndResetKey(GLFWwindow *window) {
	if (glfwGetKey(window, GLFW_KEY_KP_SUBTRACT) == GLFW_PRESS){
		model *= scale(vec3{0.9,0.9,0.9});
		setup.scaleFactor *= 0.9;
	}
	if (glfwGetKey(window, GLFW_KEY_KP_ADD) == GLFW_PRESS){
		model *= scale(vec3{10. / 9.,10. / 9.,10. / 9.});
		setup.scaleFactor *= 10. / 9.;
	}
	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS){
		setBaseModelMatrix(window);
	}
}

/**
 * @brief input linked to change view mode of model
 * @param window glfw window pointer
 */
void changeSetup(GLFWwindow *window, int key, int action) {
	(void)window;
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

/**
 * @brief input linked to light mode settings of model
 * @param window glfw window pointer
 */
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