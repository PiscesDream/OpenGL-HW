// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <vector>
 
// Include GLEW
#define GLEW_STATIC
#include <GL/glew.h>
 
// Include GLFW
#include <glfw3.h>
GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/norm.hpp>
using namespace glm;
 
// Include AntTweakBar
#define TW_STATIC 
#include <AntTweakBar.h>
 
#include <dirent.h>
#include "CMap.h"
#include "parameters.h"
#include "handlers.h"
#include "CShader.h"
#include "CTexture.h"
#include "CSkybox.h"
bool init();

// global variables
// Just for display 
vec3 gPosition( 0.0f, 0.0f, 0.0f);
quat gOrientation;
double gScaleAlpha = 1.0;
bool gPointModel = false;
bool gLineModel = false;
GLfloat gPointSize(20.0);
GLfloat gLineWidth(1.0);

// camera
vec3 gCamPosition(0.0f, 0.0f, Params::cam_height);
vec3 gCamEularAngle(0.0f, 0.0f, 0.0f);
float gCamFov = 75.0f;
vec3 gCamSpeed(0.0f, 0.0f, 0.0f);
vec3 gLightPosition(0.0f, 0.0f, 5.0f);
double gFPS = 0.0;

// objects
CObj obj_wall;
CMap map;

int main(void)
{
	if (!init())
		return EXIT_FAILURE;

	// GLFW settings 
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	glfwSetCursorPos(window, 1024/2, 768/2);
	glClearColor(47.0/256.0, 56.0/256.0, 85.0/256.0, 0.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS); 
	//glEnable(GL_CULL_FACE);

	// Create and compile our GLSL program from the shaders
	CShader shader( "StandardShading.vertexshader", "StandardShading.fragmentshader" );
	map.loadMap("./Maps/example.txt", obj_wall);
	obj_wall.loadTexture(".\\Textures\\wall1.bmp", shader);

	//CSkybox skybox(0, 0, 20, 20, 20);

	CSkybox skybox(-Params::skybox_margin, -Params::skybox_margin,
		(map.n + 1) * Params::cell_width + Params::skybox_margin,
		(map.m + 1) * Params::cell_width + Params::skybox_margin,
		Params::skybox_height);
	CObj & obj_skybox = skybox.obj;
	obj_skybox.loadTexture(".\\Textures\\skybox1.bmp", shader);


	GLuint LightID = glGetUniformLocation(shader.programID, "LightPosition_worldspace");
	//gLightPosition = vec3(map.n / 2 * Params::cell_width, map.m / 2 * Params::cell_width, skybox.height-Params::sun_margin);
	gLightPosition = vec3(10, 10, skybox.height-Params::sun_margin);
	FPS_init();
	do{
		FPS_calc(gFPS);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
 
		shader.useProgram();
		shader.calcMVP();

		glUniform3f(LightID, gLightPosition.x, gLightPosition.y, gLightPosition.z);
		{
			obj_wall.texture.activateTexture();
			obj_wall.enableAttr(shader);
			obj_wall.draw(GL_TRIANGLES);
			obj_wall.disableAttr();
		}
		{
			obj_skybox.texture.activateTexture();
			obj_skybox.enableAttr(shader);
			obj_skybox.draw(GL_TRIANGLES);
			obj_skybox.disableAttr();
		}
 
		TwDraw();
		glfwSwapBuffers(window);
		glfwPollEvents();
	} 
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0);
 
	TwTerminate();
	glfwTerminate();
 
	return 0;
}


bool init() {
	if (!glfwInit()) {
		fprintf(stderr, "Failed to initialize GLFW\n");
		return false;
	}

	glfwWindowHint(GLFW_SAMPLES, 4); // anti-alising
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(1024, 768, "Project3", NULL, NULL);
	if (window == NULL) {
		fprintf(stderr, "Failed to open GLFW window. Assure your display driver is compatible with GLFW3.3.\n");
		glfwTerminate();
		return false;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		return false;
	}

	// Initialize the GUI
	TwInit(TW_OPENGL_CORE, NULL);
	TwWindowSize(1024, 768);
	TwBar * GUI = TwNewBar("Quick Menu");
	{
		int size[] = { 320, 700 };
		TwSetParam(GUI, NULL, "size", TW_PARAM_INT32, 2, size);
		int width = 180; // pixels
		TwSetParam(GUI, NULL, "valueswidth", TW_PARAM_INT32, 1, &width);
	}
	// FPS
	TwAddVarRW(GUI, "FPS", TW_TYPE_DOUBLE, &gFPS, "group='FPS'");
	// display mode
	TwAddVarRW(GUI, "Point Model", TW_TYPE_BOOL8, &gPointModel, "group='Display Mode' ");
	TwAddVarRW(GUI, "Point Size", TW_TYPE_FLOAT, &gPointSize, "group='Display Mode' min=0.1");
	TwAddSeparator(GUI, NULL, "group='Display Mode'");
	TwAddVarRW(GUI, "Line Model", TW_TYPE_BOOL8, &gLineModel, "group='Display Mode' ");
	TwAddVarRW(GUI, "Line Width", TW_TYPE_FLOAT, &gLineWidth, "group='Display Mode' min=0.1");
	// Object Information
	TwAddVarRW(GUI, "Zoom", TW_TYPE_DOUBLE, &gScaleAlpha, "group='Object Information' min=0.01");
	TwAddVarRW(GUI, "Position", TW_TYPE_DIR3F, &gPosition, "group='Object Information'");
	TwAddVarRO(GUI, "Rotation", TW_TYPE_QUAT4F, &gOrientation, "group='Object Information'");
	// Light Position
	TwAddVarRW(GUI, "Light Position", TW_TYPE_DIR3F, &gLightPosition, "group='Light Information'");

	// Callback functions
	glfwSetMouseButtonCallback(window, (GLFWmousebuttonfun)TwEventMouseButtonGLFW); // - Directly redirect GLFW mouse button events to AntTweakBar
	glfwSetCharCallback(window, (GLFWcharfun)TwEventCharGLFW);                      // - Directly redirect GLFW char events to AntTweakBar
	glfwSetMouseButtonCallback(window, (GLFWmousebuttonfun)MouseHandler); 
	glfwSetCursorPosCallback(window, (GLFWcursorposfun)CursorHandler);   
	glfwSetScrollCallback(window, (GLFWscrollfun)ScrollHandler);  
	glfwSetKeyCallback(window, (GLFWkeyfun)KeyHandler); 

	return true;
}