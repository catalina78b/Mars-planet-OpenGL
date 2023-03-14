//
//  main.cpp
//  OpenGL Advances Lighting
//
//  Created by CGIS on 28/11/16.
//  Copyright � 2016 CGIS. All rights reserved.
//

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/matrix_inverse.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "Shader.hpp"
#include "Model3D.hpp"
#include "Camera.hpp"
#include "SkyBox.hpp"

#include <iostream>

int glWindowWidth = 1900;
int glWindowHeight = 900;
int retina_width, retina_height;
GLFWwindow* glWindow = NULL;

GLuint shadowMapFBO;
GLuint depthMapTexture;

const unsigned int SHADOW_WIDTH = 2048;
const unsigned int SHADOW_HEIGHT = 2048;

glm::mat4 model;
GLuint modelLoc;
glm::mat4 view;
GLuint viewLoc;
glm::mat4 projection;
GLuint projectionLoc;
glm::mat3 normalMatrix;
glm::mat3 ufoNormalMatrix;
GLuint normalMatrixLoc;
glm::mat4 lightRotation;

glm::mat4 ufoModel;

glm::vec3 lightDir;
GLuint lightDirLoc;
glm::vec3 lightColor;
GLuint lightColorLoc;

glm::vec3 lightPos1;
GLuint lightPosLoc1;

glm::vec3 lightPos2;
GLuint lightPosLoc2;

glm::vec3 lightPos3;
GLuint lightPosLoc3;

glm::vec3 lightPos4;
GLuint lightPosLoc4;

glm::vec3 lightPos5;
GLuint lightPosLoc5;

GLuint isLightLoc;

glm::vec3 cameraPosition;

float alpha;
GLuint alphaLoc;
int isLight=1;
int presentation;
int cnt = 0;


gps::SkyBox mySkyBox;
gps::Shader skyboxShader;



glm::mat4 lightView;
const GLfloat near_plane = 0.1f, far_plane = 5.0f;
glm::mat4 lightProjection;
glm::mat4 lightSpaceTrMatrix;

gps::Camera myCamera(
				glm::vec3(0.0f, 1.0f, 5.0f), 
				glm::vec3(0.0f, 0.0f, 2.0f),
				glm::vec3(0.0f, 1.0f, 0.0f));
float cameraSpeed = 0.01f;

bool pressedKeys[1024];
float angleY = 0.0f;
float yaw = 1.5f;
float pitch = 0;

GLfloat lightAngle;

gps::Model3D scene;
gps::Model3D beers;
gps::Model3D ice;
gps::Model3D glass;
gps::Model3D ufo;

gps::Shader myCustomShader;

GLfloat ufoPosition = 0.0f;
GLfloat step = 0.04f;

bool showDepthMap;

std::vector<const GLchar*> faces;

float delta = 0;
float movementSpeed = 2; // units per second
void updateDelta(double elapsedSeconds) {
	delta = delta + movementSpeed * elapsedSeconds;
}
double lastTimeStamp = glfwGetTime();



GLenum glCheckError_(const char *file, int line) {
	GLenum errorCode;
	while ((errorCode = glGetError()) != GL_NO_ERROR)
	{
		std::string error;
		switch (errorCode)
		{
		case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
		case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
		case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
		case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
		case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
		case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
		case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
		}
		std::cout << error << " | " << file << " (" << line << ")" << std::endl;
	}
	return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__)

void windowResizeCallback(GLFWwindow* window, int width, int height) {
	fprintf(stdout, "window resized to width: %d , and height: %d\n", width, height);
	//TODO	
}

void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key == GLFW_KEY_M && action == GLFW_PRESS)
		showDepthMap = !showDepthMap;

	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
			pressedKeys[key] = true;
		else if (action == GLFW_RELEASE)
			pressedKeys[key] = false;
	}
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos) {

}

void processMovement() {

	if (pressedKeys[GLFW_KEY_Q]) {
		presentation = 1;
	}
	if (pressedKeys[GLFW_KEY_H]) {
		isLight = 0;
	}

	if (pressedKeys[GLFW_KEY_J]) {
		isLight = 1;
	}
	if (pressedKeys[GLFW_KEY_W]) {
		myCamera.move(gps::MOVE_FORWARD, cameraSpeed);
	}

	if (pressedKeys[GLFW_KEY_S]) {
		myCamera.move(gps::MOVE_BACKWARD, cameraSpeed);
	}

	if (pressedKeys[GLFW_KEY_LEFT]) {
		myCamera.move(gps::MOVE_LEFT, cameraSpeed);
	}

	if (pressedKeys[GLFW_KEY_RIGHT]) {
		myCamera.move(gps::MOVE_RIGHT, cameraSpeed);
	}
	if (pressedKeys[GLFW_KEY_UP]) {
		myCamera.moveUp(cameraSpeed);
	}

	if (pressedKeys[GLFW_KEY_DOWN]) {
		myCamera.moveDown(cameraSpeed);
	}
	float sensitivity = 0.01f;

	if (pressedKeys[GLFW_KEY_R]) {
		yaw += 1 * sensitivity;
		myCamera.rotate(pitch, yaw);
	}
	if (pressedKeys[GLFW_KEY_T]) {
		yaw -= 1 * sensitivity;
		myCamera.rotate(pitch, yaw);
	}
	
	if (pressedKeys[GLFW_KEY_P]) { //wireframe
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	if (pressedKeys[GLFW_KEY_O]) { //solid
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	if (pressedKeys[GLFW_KEY_U]) { //smooth
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glEnable(GL_POLYGON_SMOOTH);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_DST_ALPHA);
	}

	if (pressedKeys[GLFW_KEY_Y]) { //polygonal
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
	}

}

bool initOpenGLWindow()
{
	if (!glfwInit()) {
		fprintf(stderr, "ERROR: could not start GLFW3\n");
		return false;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SRGB_CAPABLE, GLFW_TRUE);
	glfwWindowHint(GLFW_SAMPLES, 4);

	glWindow = glfwCreateWindow(glWindowWidth, glWindowHeight, "OpenGL Shader Example", NULL, NULL);
	if (!glWindow) {
		fprintf(stderr, "ERROR: could not open window with GLFW3\n");
		glfwTerminate();
		return false;
	}

	glfwSetWindowSizeCallback(glWindow, windowResizeCallback);
	glfwSetKeyCallback(glWindow, keyboardCallback);
	glfwSetCursorPosCallback(glWindow, mouseCallback);
	//glfwSetInputMode(glWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glfwMakeContextCurrent(glWindow);

	glfwSwapInterval(1);

	// start GLEW extension handler
	glewExperimental = GL_TRUE;
	glewInit();

	// get version info
	const GLubyte* renderer = glGetString(GL_RENDERER); // get renderer string
	const GLubyte* version = glGetString(GL_VERSION); // version as a string
	printf("Renderer: %s\n", renderer);
	printf("OpenGL version supported %s\n", version);

	//for RETINA display
	glfwGetFramebufferSize(glWindow, &retina_width, &retina_height);

	return true;
}

void initOpenGLState()
{
	glClearColor(0.3, 0.3, 0.3, 1.0);
	glViewport(0, 0, retina_width, retina_height);

	glEnable(GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
	glEnable(GL_CULL_FACE); // cull face
	glCullFace(GL_BACK); // cull back face
	glFrontFace(GL_CCW); // GL_CCW for counter clock-wise

	glEnable(GL_FRAMEBUFFER_SRGB);
}

void initObjects() {
	scene.LoadModel("objects/scene_final.obj");
	ice.LoadModel("objects/ice_object.obj");
	beers.LoadModel("objects/beri.obj");
	glass.LoadModel("objects/sticla_sparta.obj");
	ufo.LoadModel("objects/ufo_01.obj");

}

void initShaders() {
	myCustomShader.loadShader("shaders/shaderStart.vert", "shaders/shaderStart.frag");
	myCustomShader.useShaderProgram();
	mySkyBox.Load(faces);
	skyboxShader.loadShader("shaders/skyboxShader.vert", "shaders/skyboxShader.frag");
	skyboxShader.useShaderProgram();

}

void initUniforms() {

	myCustomShader.useShaderProgram();


	model = glm::mat4(1.0f);
	modelLoc = glGetUniformLocation(myCustomShader.shaderProgram, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	view = myCamera.getViewMatrix();
	viewLoc = glGetUniformLocation(myCustomShader.shaderProgram, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

	normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
	normalMatrixLoc = glGetUniformLocation(myCustomShader.shaderProgram, "normalMatrix");
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

	projection = glm::perspective(glm::radians(45.0f), (float)retina_width / (float)retina_height, 0.1f, 1000.0f);
	projectionLoc = glGetUniformLocation(myCustomShader.shaderProgram, "projection");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

	cameraPosition = myCamera.getPosition();
	glUniform3fv(glGetUniformLocation(myCustomShader.shaderProgram, "viewPos"), 1,glm::value_ptr(cameraPosition));

	//set the light direction (direction towards the light)
	lightDir = glm::vec3(0.0f, 1.0f, 1.0f);
	lightRotation = glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f));
	lightDirLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightDir");
	glUniform3fv(lightDirLoc, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view * lightRotation)) * lightDir));

	//set light color
	lightColor = glm::vec3(1.0f, 1.0f, 1.0f); //white
	lightColorLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightColor");
	glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));

	//set light pos1
	lightPos1 = glm::vec3(-2.61f, -0.014f, 6.76f);
	lightPosLoc1 = glGetUniformLocation(myCustomShader.shaderProgram, "lightPos1");
	glUniform3fv(lightPosLoc1, 1, glm::value_ptr(lightPos1));

	//set light pos2
	lightPos2 = glm::vec3(-2.48f, -0.039f, 9.6f); 
	lightPosLoc2 = glGetUniformLocation(myCustomShader.shaderProgram, "lightPos2");
	glUniform3fv(lightPosLoc2, 1, glm::value_ptr(lightPos2));

	//set light pos3
	lightPos3 = glm::vec3(2.38f,-0.96f, -4.42f);
	lightPosLoc3 = glGetUniformLocation(myCustomShader.shaderProgram, "lightPos3");
	glUniform3fv(lightPosLoc3, 1, glm::value_ptr(lightPos3));

	lightPos4 = glm::vec3(2.86f, -0.3f, -3.21f);
	lightPosLoc4 = glGetUniformLocation(myCustomShader.shaderProgram, "lightPos4");
	glUniform3fv(lightPosLoc4, 1, glm::value_ptr(lightPos4));

}

void initFBO() {
	//TODO - Create the FBO, the depth texture and attach the depth texture to the FBO
	glGenFramebuffers(1, &shadowMapFBO);
	glGenTextures(1, &depthMapTexture);
	glBindTexture(GL_TEXTURE_2D, depthMapTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	//attach texture to FBO
	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapTexture,0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

glm::mat4 computeLightSpaceTrMatrix() {
	//TODO - Return the light-space transformation matrix
	lightView = glm::lookAt(lightDir, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    lightProjection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, near_plane, far_plane);
	lightSpaceTrMatrix = lightProjection * lightView;
	return lightSpaceTrMatrix;
}

void drawObjects(gps::Shader shader, bool depthPass) {
		
	shader.useShaderProgram();
	
	model = glm::rotate(glm::mat4(1.0f), glm::radians(angleY), glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

	// do not send the normal matrix if we are rendering in the depth map
	if (!depthPass) {
		normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
		glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	}

	scene.Draw(shader);

	isLightLoc = glGetUniformLocation(myCustomShader.shaderProgram, "isLight");
	glUniform1i(isLightLoc, isLight);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	alpha = 0.5;
	alphaLoc = glGetUniformLocation(shader.shaderProgram, "alpha");
	glUniform1f(alphaLoc, alpha);
	ice.Draw(shader);	
	glDisable(GL_BLEND);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	alpha = 0.7;
	alphaLoc = glGetUniformLocation(shader.shaderProgram, "alpha");
	glUniform1f(alphaLoc, alpha);
	beers.Draw(shader);
	glDisable(GL_BLEND);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	alpha = 0.7;
	alphaLoc = glGetUniformLocation(shader.shaderProgram, "alpha");
	glUniform1f(alphaLoc, alpha);
	glass.Draw(shader);
	glDisable(GL_BLEND);

	double currentTimeStamp = glfwGetTime();
	updateDelta(currentTimeStamp - lastTimeStamp);
	lastTimeStamp = currentTimeStamp;

	ufoPosition += step;
	if (ufoPosition > 2.0f || ufoPosition < 0.0f) {
		step = -step;
	}

	model = 
		glm::translate(glm::mat4(1.0f), glm::vec3(2.0f, ufoPosition, 3.4f)) *
		glm::rotate(glm::mat4(1.0f), 0.3f*delta, glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
	normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
	if (!depthPass) {
		model = glm::mat3(glm::inverseTranspose(model));
		glUniformMatrix3fv(glGetUniformLocation(shader.shaderProgram, "normalMatrix"), 1, GL_FALSE, glm::value_ptr(normalMatrix));
	}
	ufo.Draw(shader);
	
}
void animation() {
	if (presentation) {
		if (cnt <= 50) {
			cnt++;
			isLight = 1;
			pressedKeys[GLFW_KEY_S] = true;
			pressedKeys[GLFW_KEY_DOWN] = true;

		}
		if (cnt > 50 && cnt <= 300)
		{
			cnt++;
			pressedKeys[GLFW_KEY_DOWN] = false;

		}
		if (cnt > 300 && cnt <= 450)
		{
			cnt++;
			pressedKeys[GLFW_KEY_LEFT] = true;
			pressedKeys[GLFW_KEY_UP] = false;

		}
		if (cnt > 450 && cnt<=500)
		{
			cnt++;
			isLight = 0;
			pressedKeys[GLFW_KEY_S] = false;
			pressedKeys[GLFW_KEY_DOWN] = true;
			pressedKeys[GLFW_KEY_W] = true;
			pressedKeys[GLFW_KEY_LEFT] = false;
		
		}
		if (cnt > 500 && cnt<=650)
		{
			cnt++;
			pressedKeys[GLFW_KEY_DOWN] = false;
			pressedKeys[GLFW_KEY_W] = true;
			pressedKeys[GLFW_KEY_UP] = false;
			pressedKeys[GLFW_KEY_LEFT] = false;
			
			
		}

		if (cnt > 650 && cnt <= 700)
		{
			cnt++;
			pressedKeys[GLFW_KEY_DOWN] = false;
			pressedKeys[GLFW_KEY_W] = true;
			pressedKeys[GLFW_KEY_UP] = false;
			pressedKeys[GLFW_KEY_LEFT] = false;


		}
		if (cnt > 700  && cnt<=750)
		{
			cnt++;
			pressedKeys[GLFW_KEY_LEFT] = false;
			pressedKeys[GLFW_KEY_W] = true;
			pressedKeys[GLFW_KEY_UP] = false;
			pressedKeys[GLFW_KEY_RIGHT] = false;

		}
		if (cnt > 750 && cnt <= 920)
		{
			cnt++;
			pressedKeys[GLFW_KEY_LEFT] = false;
			pressedKeys[GLFW_KEY_W] = true;
			pressedKeys[GLFW_KEY_UP] = true;
			pressedKeys[GLFW_KEY_RIGHT] = true;

		}
		if (cnt > 920 && cnt <= 950)
		{
			cnt++;
			pressedKeys[GLFW_KEY_LEFT] = false;
			pressedKeys[GLFW_KEY_W] = false;
			pressedKeys[GLFW_KEY_UP] = false;
			pressedKeys[GLFW_KEY_RIGHT] = true;

		}
		if (cnt > 950)
		{
			cnt++;
			pressedKeys[GLFW_KEY_W] = false;
			pressedKeys[GLFW_KEY_UP] = false;
			pressedKeys[GLFW_KEY_RIGHT] = false;
			presentation = 0;
		}
	}
}

void renderScene() {


		glViewport(0, 0, retina_width, retina_height);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glClearColor(0.5, 0.5, 0.5, 1.0);

		myCustomShader.useShaderProgram();

		view = myCamera.getViewMatrix();
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
				
		lightRotation = glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniform3fv(lightDirLoc, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view * lightRotation)) * lightDir));


		glUniformMatrix4fv(glGetUniformLocation(myCustomShader.shaderProgram, "lightSpaceTrMatrix"),
			1,
			GL_FALSE,
			glm::value_ptr(computeLightSpaceTrMatrix()));

		drawObjects(myCustomShader, false);

		mySkyBox.Draw(skyboxShader,view,projection);
}
void initFaces()
{
	faces.push_back("textures/skybox/marslike01rt.tga");
	faces.push_back("textures/skybox/marslike01lf.tga");
	faces.push_back("textures/skybox/marslike01up.tga");
	faces.push_back("textures/skybox/marslike01dn.tga");
	faces.push_back("textures/skybox/marslike01bk.tga");
	faces.push_back("textures/skybox/marslike01ft.tga");
}
void cleanup() {
	glDeleteTextures(1,& depthMapTexture);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDeleteFramebuffers(1, &shadowMapFBO);
	glfwDestroyWindow(glWindow);
	//close GL context and any other GLFW resources
	glfwTerminate();
}

int main(int argc, const char * argv[]) {

	if (!initOpenGLWindow()) {
		glfwTerminate();
		return 1;
	}

	initFaces();
	initOpenGLState();
	initObjects();
	initShaders();
	initUniforms();
	initFBO();

	while (!glfwWindowShouldClose(glWindow)) {
		processMovement();
		renderScene();		
		animation();
		glfwPollEvents();
		glfwSwapBuffers(glWindow);
	}

	cleanup();

	return 0;
}
