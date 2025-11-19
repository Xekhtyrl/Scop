#include "Includes/header.h"
#include "Includes/vml.hpp"
#include "Camera.hpp"
#include "Model.hpp"
#include <globals.hpp>
#include "Includes/imgui/imgui.h"
#include "Includes/imgui/imgui_impl_glfw.h"
#include "Includes/imgui/imgui_impl_opengl3.h"

using namespace vml;

const unsigned int SCR_WIDTH = 1400;
const unsigned int SCR_HEIGHT = 1200;
float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame
bool firstMouse = true;
float lastX =  SCR_WIDTH / 2.0;
float lastY =  SCR_HEIGHT / 2.0;
Camera camera(vec3({0.,0.,3.}));
Model object;
mat4 model;
Setup setup = Setup();
vec3 center;


void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void setBaseModelMatrix(GLFWwindow* window) {
    model = identity<float,4>();

    vec3 rawMin = object.min();
    vec3 rawMax = object.max();

    vec3 rawCenter = (rawMin + rawMax) * 0.5f;
    vec3 rawSize = rawMax - rawMin;

    float maxExtent = std::max({ rawSize[0], rawSize[1], rawSize[2] });

	setup.scaleFactor = 1.0f;
    mat4 normalization =
          scale(vec3{1.0f / maxExtent})
        * translation(rawCenter * -1.0f);

    model = normalization;

    vec4 normalizedCenter4 = normalization * vec4(rawCenter, 1.0f);
    center = vec3({normalizedCenter4[0], normalizedCenter4[1], normalizedCenter4[2]});
	camera.resetCamera(window);
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
	changeLightSettings(window);
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
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
    float yoffset = lastY - ypos;

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

void setup_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	changeSetup(window, key, action);
}

void setupCustomTexture() {
	setup.custom.loadTexture("Textures/BlackLodge.png", {.params = {GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR}});
}

GLFWwindow* initWindow() {
	glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	return glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
}

void initImgui(GLFWwindow* window) {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	ImGui::StyleColorsDark();

	// Initialize GLFW + OpenGL3 backends
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");
}

void defineMatrices(Shader shad, Camera& camera) {
	mat4 view = camera.GetViewMatrix();
	mat4 projection = perspective(radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1, 100.);

	int viewLoc = glGetUniformLocation(shad.getID(), "view");
	glUniformMatrix4fv(viewLoc, 1, GL_TRUE, view.data);
	int projectionLoc = glGetUniformLocation(shad.getID(), "projection");
	glUniformMatrix4fv(projectionLoc, 1, GL_TRUE, projection.data);
	int modelLoc = glGetUniformLocation(shad.getID(), "model");
	glUniformMatrix4fv(modelLoc, 1, GL_TRUE, model.data);
}

void createUIImgui(){
	const char* modeName = "Solid"; // or dynamic
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	ImGui::Begin("Settings", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

	ImGui::Text("Rendering mode: %s", modeName);
	ImGui::SliderFloat("Scale", &setup.scaleFactor, 0.1f, 10.0f);
	ImGui::Checkbox("Show Faces (F)", &setup.showFaces);
	ImGui::Checkbox("Show Lines (L)", &setup.showLines);
	ImGui::Checkbox("Show Points (P)", &setup.showPoints);
	ImGui::Checkbox("Custom Texture (T)", &setup.applyCustomTexture);

	ImGui::Text("\nLegend:\n\n");
	ImGui::Text("Light Settings:\n");
	ImGui::TextColored({0,0.8,0.8,1}, "light position (1, 2, 3):\n\t %f , %f, %f", setup.lightPos[0], setup.lightPos[1], setup.lightPos[2]);
	ImGui::TextColored({0,0.8,0.8,1}, "light color (4, 5, 6):\n\t %f , %f, %f", setup.lightColor[0], setup.lightColor[1], setup.lightColor[2]);
	ImGui::TextColored({0,0.8,0.8,1}, "view position (7, 8, 9):\n\t %f , %f, %f", setup.viewPos[0], setup.viewPos[1], setup.viewPos[2]);
	ImGui::Text("Reset light settings (0)\n");

	ImGui::Text("\nModel Controls:\n");
	ImGui::Text("\tRotation X: left Arrow, right Arrow\n");
	ImGui::Text("\tRotation Y: up Arrow, down Arrow\n");
	ImGui::Text("\tTranslation Y: NUM2, NUM8\n");
	ImGui::Text("\tTranslation X: NUM4, NUM6\n");
	ImGui::Text("\tTranslation Z: NUM1, NUM9\n");
	ImGui::Text("\nCamera Controls:\n");
	ImGui::Text("\tMouse, A, W, S, D\n");
	
	ImGui::TextColored({0.8,0.8,0,1} ,"Reset Position & Camera (R)\n");

	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

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
	
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}
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
	setupCustomTexture();
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	try {
		Shader shad("ShadersFiles/FinalVertexTexShad.glsl", "ShadersFiles/FinalFragTexShad.glsl");
		object = Model(argv[argc - 1]);
		std::cout<< "object loaded" <<std::endl;
		setBaseModelMatrix(window);

		while(!glfwWindowShouldClose(window))
		{
			
			float currentFrame = glfwGetTime();
			deltaTime = currentFrame - lastFrame;
			lastFrame = currentFrame; 
			processInput(window, camera);
			// Set the clear color (RGBA)
        	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			createUIImgui();
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
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate();
    return 0;
}
