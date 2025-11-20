#include "Includes/header.h"

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

void defineMatrices(Shader& shad, Camera& camera) {
	mat4 view = camera.GetViewMatrix();
	mat4 projection = perspective(radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1, 100.);

	int viewLoc = glGetUniformLocation(shad.getID(), "view");
	glUniformMatrix4fv(viewLoc, 1, GL_TRUE, view.data);
	int projectionLoc = glGetUniformLocation(shad.getID(), "projection");
	glUniformMatrix4fv(projectionLoc, 1, GL_TRUE, projection.data);
	int modelLoc = glGetUniformLocation(shad.getID(), "model");
	glUniformMatrix4fv(modelLoc, 1, GL_TRUE, model.data);
}