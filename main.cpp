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
			glClearColor(0.75, 0.75f, 0.6f, 1.0f);
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

void setObjName(std::string arg) {
	setup.modelName = arg.substr(arg.find_last_of('/') + 1);
}

int main(int argc, char **argv)
{
	std::string obj = argv[1];
	std::ofstream log;
	log.open("err.log");
	log << "log file open with" << obj <<std::endl;
	if (argc < 2){
		log << "Object needed in parameter. No more no less." << std::endl;
		// return -1;
		obj = "Resources/42.obj";
	}
	setObjName(argv[1]);
	GLFWwindow* window = initWindow(setup.modelName);
	if (window == NULL)
	{
		log << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	log << "window opened" << std::endl;
	glfwMakeContextCurrent(window);
	log << "window Context created" << std::endl;
	
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		log << "Failed to initialize GLAD" << std::endl;
		glfwDestroyWindow(window);
		glfwTerminate();
		return -1;
	}

	log << "glad loaded" << std::endl;
	setupOpenGL(window);
	log << "OpenGL setuped" << std::endl;
	setupCustomTexture(argc, argv);
	log << "Custom Texture Loaded" << std::endl;
	try {
		Shader shad("ShadersFiles/FinalVertexTexShad.glsl", "ShadersFiles/FinalFragTexShad.glsl");
		log << "shader created" << std::endl;
		object = Model((char *)obj.c_str());
		log << "model created" << std::endl;
		setBaseModelMatrix(window);
		log << "before loop" << std::endl;
		renderLoop(window, shad);
	}
	catch(std::exception& e){
		log << e.what() << std::endl;
	}
	cleanProgram(window);
	log << "program closed without error or exception?" << std::endl;
	log.close();
	return 0;
}
