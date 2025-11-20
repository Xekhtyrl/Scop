#include "Includes/header.h"
#include "Includes/vml.hpp"
#include "Camera.hpp"
#include "Model.hpp"

#include "Includes/imgui/imgui.h"
#include "Includes/imgui/imgui_impl_glfw.h"
#include "Includes/imgui/imgui_impl_opengl3.h"


using namespace vml;

const unsigned int SCR_WIDTH = 1400;
const unsigned int SCR_HEIGHT = 1200;
float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame
float lastX =  SCR_WIDTH / 2.0;
float lastY =  SCR_HEIGHT / 2.0;
Camera camera(vec3({0.,0.,3.}));
Model object;
mat4 model;
Setup setup = Setup();
vec3 center;


void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	(void)window;
	glViewport(0, 0, width, height);
}

void setupCustomTexture(int argc, char **argv) {
	if (argc < 3)
		setup.custom.loadTexture("Textures/BlackLodge.png");
	else{
		setup.custom.loadTexture(argv[2]);
		std::cout << "You have inserted a custom Texture Path. Be warned that it might not appears correctly depending on the Texture selected" <<std::endl;
	}
}

void renderLoop(GLFWwindow *window, Shader& shader) {
	while(!glfwWindowShouldClose(window))
		{
			
			float currentFrame = glfwGetTime();
			deltaTime = currentFrame - lastFrame;
			lastFrame = currentFrame; 
			processInput(window, camera);
			// Set the clear color (RGBA)
			glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			shader.use();
			defineMatrices(shader, camera);
			
			object.Draw(shader);
			
			createUIImgui();
			glfwSwapBuffers(window);
			glfwPollEvents();
			
		}
}

void setupOpenGL(GLFWwindow *window) {
	initImgui(window);
	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetKeyCallback(window, setup_callback);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	// Make window visible before setting cursor
	glfwShowWindow(window);

	// Center cursor after window is visible
	glfwSetCursorPos(window, SCR_WIDTH / 2.0, SCR_HEIGHT / 2.0);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
}

void cleanProgram(GLFWwindow *window) {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate();
}

int main(int argc, char **argv)
{
	if (argc < 2){
		std::cerr << "Object needed in parameter. No more no less.";
		return -1;
	}
	GLFWwindow* window = initWindow("Scop42");
	if (window == NULL)
	{
		std::cerr << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cerr << "Failed to initialize GLAD" << std::endl;
		glfwDestroyWindow(window);
		glfwTerminate();
		return -1;
	}

	setupOpenGL(window);
	setupCustomTexture(argc, argv);
	try {
		Shader shad("ShadersFiles/FinalVertexTexShad.glsl", "ShadersFiles/FinalFragTexShad.glsl");
		object = Model(argv[1]);
		setBaseModelMatrix(window);

		renderLoop(window, shad);
	}
	catch(std::exception& e){
		std::cerr << e.what() << std::endl;
	}
	cleanProgram(window);
	return 0;
}
