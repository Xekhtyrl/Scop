#include "Includes/header.h"

#include "Includes/imgui/imgui.h"
#include "Includes/imgui/imgui_impl_glfw.h"
#include "Includes/imgui/imgui_impl_opengl3.h"


/**
 * @brief set of functions needed for initializing the GLFW window with a set name and size
 * 
 * @param name window name that will appears on top of it
 * 
 * @return GLFWwindow pointer of the created window
 */
GLFWwindow* initWindow(std::string name) {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	return glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, name.size() > 0 ? name.c_str() : "Scop42", NULL, NULL);
}

/**
 * @brief set of function to initialize the use of imgui in the current GLFW window
 * 
 * @param window pointer to the GLFW window
 */
void initImgui(GLFWwindow* window) {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::GetIO();
	ImGui::StyleColorsDark();

	// Initialize GLFW + OpenGL3 backends
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");
}

/**
 * @brief create and draw Imgui frame on the window and fill it with the details of the program
 * 
 * Give details on the view mode activated, the light parameter and the legend on the controls
 */
void createUIImgui(){
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	ImGui::Begin("Settings", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

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
	ImGui::Text("\tRotation Z (CTRL +): left Arrow, right Arrow\n");
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