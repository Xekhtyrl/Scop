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

/**
 * @brief resize window frame function
 * 
 * @param window glfw window pointer
 * @param width new width of window
 * @param height new height of window
 */
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	(void)window;
	glViewport(0, 0, width, height);
}


/** @brief set a Custom Texture to the program, or a dafault one.
 * 
 *	@param argc number of argument given when the program is launch (main parameters)
 *	@param argv arguments given when the program is launch (main parameters)
 *	@param log out stream for the log messages
*/
void setupCustomTexture(int argc, char **argv, std::ostream& log) {
	if (argc < 3)
		setup.custom.loadTexture("Textures/BlackLodge.png");
	else{
		setup.custom.loadTexture(argv[2]);
		log << "You have inserted a custom Texture Path. Be warned that it might not appears correctly depending on the Texture selected" <<std::endl;
	}
}

/**
 * @brief rendering loop function that will, in order: call functions to process input, redefine based on input the model matrix, draw each meshes in the model and redraw the UI imgui window.
 * 
 * @param window glfw window pointer.
 * @param shader shader class needed beforehand to draw the meshes with and send update to the program on the model.
 */
void renderLoop(GLFWwindow *window, Shader& shader) {
	
	while(!glfwWindowShouldClose(window))
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame; 
		processInput(window);
		// Set the clear color (RGBA)
		glClearColor(0.75, 0.75f, 0.6f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		shader.use();
		defineMatrices(shader);
		
		object.Draw(shader);
		
		createUIImgui();
		glfwSwapBuffers(window);
		glfwPollEvents();
		
	}
}

/**
 * @brief setup the window and legend details as well as callbacks functions for mouse and keyboard
 * 
 * @param window pointer to the glfw window
 */
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

/**
 * @brief clean function called at the end of program(with or without errors) that close and free the imGui context window and the GLFW window as well as terminate it.
 * 
 *	@param window the GLFW window pointer
 */
void cleanProgram(GLFWwindow *window) {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate();
}

/** @brief simple function to get the .obj name from path and put it in the Setup Struct
 * 
 * @param arg path name to extract name from
*/
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
		log << "Program ended prematurely. Object needed in parameter. No more no less." << std::endl;
		log.close();
		return -1;
	}
	setObjName(argv[1]);
	GLFWwindow* window = initWindow(setup.modelName);
	if (window == NULL)
	{
		log << "Program ended prematurely. Failed to create GLFW window." << std::endl;
		glfwTerminate();
		log.close();
		return -1;
	}
	log << "Window opened successfully!" << std::endl;
	glfwMakeContextCurrent(window);
	log << "Window context created successfully!" << std::endl;
	
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		log << "Program ended prematurely. Failed to initialize GLAD." << std::endl;
		glfwDestroyWindow(window);
		glfwTerminate();
		return -1;
	}
	
	try {
		log << "Glad loaded successfully" << std::endl;
		setupOpenGL(window);
		log << "OpenGL setuped" << std::endl;
		setupCustomTexture(argc, argv, log);
		log << "Custom Texture " <<  setup.custom.path() << " Loaded Successfully" << std::endl;

		Shader shad("ShadersFiles/FinalVertexTexShad.glsl", "ShadersFiles/FinalFragTexShad.glsl");
		log << "Shader created Successfully" << std::endl;
		object = Model((char *)obj.c_str());
		log << "Kodel created Successfully" << std::endl;
		setBaseModelMatrix(window);
		renderLoop(window, shad);
	}
	catch(std::exception& e){
		log << "Exception catched: " << e.what() << std::endl;
		cleanProgram(window);
		return -1;
	}
	cleanProgram(window);
	log << "program closed without error or exception" << std::endl;
	log.close();
	return 0;
}
