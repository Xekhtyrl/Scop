#include "Includes/header.h"
#include "Includes/vml.hpp"
#include "Camera.hpp"
#include "Model.hpp"
#include <globals.hpp>

using namespace vml;

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame
bool firstMouse = true;
float lastX =  SCR_WIDTH / 2.0;
float lastY =  SCR_HEIGHT / 2.0;
Camera camera(vec3({0.,0.,3.}));
Model object;
mat4 model;


void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void setBaseModelMatrix(){
	model = identity<float,4>();
	// vec3 size = object.max() - object.min();
	// vec3 center = (object.max() + object.min());
	// center = vec3{center[0] / size[0], center[1] / size[1], center[2] / size[2]} * 0.5;

	// float maxExtent = std::max(size[0], std::max(size[1], size[2]));

	// mat4 normalization = translation(center * -1);  
	// normalization *= scale(vec3{1.0f / maxExtent});     	// uniform scale
	
	// model = normalization;
}

void processInput(GLFWwindow *window, Camera& camera)
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
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
	// Camera* camera = static_cast<Camera*>(glfwGetWindowUserPointer(window));
	// if (!camera)
	// 	return;
	float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);
	
    if (firstMouse)
    {
		glfwSetCursorPos(window, SCR_WIDTH / 2.0, SCR_HEIGHT / 2.0);
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	// Camera* camera = static_cast<Camera*>(glfwGetWindowUserPointer(window));
    // if (camera)
        camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

GLFWwindow* initWindow() {
	glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	return glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
}

void defineMatrices(Shader shad, Camera& camera) {
	mat4 view = camera.GetViewMatrix(); //identity<float,4>(); //
	mat4 projection = perspective(radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1, 100.); //identity<float,4>(); //
	// model.print();
	// model.print();
	  // scale objectect
	// model.print();


	int viewLoc = glGetUniformLocation(shad.getID(), "view");
	glUniformMatrix4fv(viewLoc, 1, GL_TRUE, view.data);
	int projectionLoc = glGetUniformLocation(shad.getID(), "projection");
	glUniformMatrix4fv(projectionLoc, 1, GL_TRUE, projection.data);
	int modelLoc = glGetUniformLocation(shad.getID(), "model");
	glUniformMatrix4fv(modelLoc, 1, GL_TRUE, model.data);
}

int main(int argc, char **argv)
{
	std::cout << argc << argv[0] << std::endl;
	if (argc < 2){
		std::cerr << "Object needed in parameter. No more no less.";
		return -1;
	}

	GLFWwindow* window = initWindow();
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	// glfwSetWindowUserPointer(window, &camera);
	
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glViewport(0, 0, 800, 600);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	
	glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		// Make window visible before setting cursor
	glfwShowWindow(window);

	// Center cursor after window is visible
	glfwSetCursorPos(window, SCR_WIDTH / 2.0, SCR_HEIGHT / 2.0);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	GLenum err;
	try {
		Shader shad("ShadersFiles/FinalVertexTexShad.glsl", "ShadersFiles/FinalFragTexShad.glsl");
		object = Model(argv[argc - 1]);
		setBaseModelMatrix();
		// std::cout << "print Vertices MEshes" << std::endl;
		// for (auto& x : obj.getMeshes()){
		// 	for (auto& y: x.vertices()){
		// 		y.Normal.print();
		// 		y.Position.print();
		// 		y.TexCoords.print();
		// 	}
		// 	for (auto& y : x.indices()){
		// 		std::cout << y << " ";
		// 	}
		// 	std::cout << "\n";
		// }

		while(!glfwWindowShouldClose(window))
		{

			float currentFrame = glfwGetTime();
			deltaTime = currentFrame - lastFrame;
			lastFrame = currentFrame; 
			processInput(window, camera);
			// Set the clear color (RGBA)
        	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			
			shad.use();
			defineMatrices(shad, camera);

			object.Draw(shad);
			glfwSwapBuffers(window);
			glfwPollEvents();
			
		}
	}
	catch(std::exception& e){
		std::cerr << e.what() << std::endl;
		exit(0);
	}
	glfwDestroyWindow(window);
	glfwTerminate();
    return 0;
}
