// Include standard headers
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
 
// Include GLEW
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
#include <AntTweakBar.h>
 
#include <dirent.h>

#include <CObj.h>
#include <CTexture.h>
#include <CShader.h>
#include "handlers.h"
#include "parameters.h"
#include "CMap.h"
#include "CSkybox.h"
#include "CParticle.h"
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
CMap map;

int main(void)
{
	if (!init())
		return EXIT_FAILURE;

	// GLFW settings 
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS); 
	//glEnable(GL_CULL_FACE);

	// Create and compile our GLSL program from the shaders
	CShader shader( "StandardShading.vertexshader", "StandardShading.fragmentshader" );
	shader.setLight(140000.0f);
	CObj obj_wall;
	CObj obj_ground;
	CObj obj_target;
	//map.parseMap("./Maps/example.txt");
	map.generateMap(20, 20, 0.8);
	map.render(obj_wall, obj_ground);
	map.print();
	obj_wall.loadTexture(".\\Textures\\wall1.bmp", shader);
	obj_ground.loadTexture(".\\Textures\\rock1.bmp", shader);

	obj_target.loadObj(".\\Models\\Cube.obj");
	obj_target.loadTexture(".\\Textures\\uvmap.DDS", shader);
	obj_target.modelMatrix = glm::translate(obj_target.modelMatrix, vec3(map.ex*Params::cell_width, map.ey*Params::cell_width, map.maph[map.ex][map.ey] * 2+2*Params::cell_height));

	//CSkybox skybox(0, 0, 20, 20, 20);

	CSkybox skybox(-Params::skybox_margin, -Params::skybox_margin,
		(map.n + 1) * Params::cell_width + Params::skybox_margin,
		(map.m + 1) * Params::cell_width + Params::skybox_margin,
		Params::skybox_height);
	CObj & obj_skybox = skybox.obj;
	obj_skybox.loadTexture(".\\Textures\\skybox1.bmp", shader);

	printf("%d %d\n", map.n, map.m);
	CParticle obj_snow(Params::p_count, Params::p_rate,
		vec2(0, (map.n + 1) * Params::cell_width),
		vec2(0, (map.m + 1) * Params::cell_width),
		vec2(0, Params::skybox_height-Params::sun_margin), 
		vec3(0, 0, -Params::p_speed), 
		vec3(0, 0, -Params::p_noise));
	obj_snow.loadTexture(".\\Textures\\snow.bmp", shader);

	/*
	CObj obj_spongebob;
	obj_spongebob.loadObj(".\\Models\\SpongeBob_obj\\Spongebob.obj");
	obj_spongebob.loadTexture(".\\Models\\SpongeBob_obj\\Spongebob.bmp", shader);
	*/

	GLuint LightID = glGetUniformLocation(shader.programID, "LightPosition_worldspace");
	//gLightPosition = vec3(map.n / 2 * Params::cell_width, map.m / 2 * Params::cell_width, skybox.height-Params::sun_margin);
	gLightPosition = vec3(10, 10, skybox.height-Params::sun_margin);
	FPS_init();
	do{
		FPS_calc(gFPS);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
 
		shader.useProgram();

		glUniform3f(LightID, gLightPosition.x, gLightPosition.y, gLightPosition.z);

		movementCalc(shader.ProjectionMatrix, shader.ViewMatrix);

		shader.calcMVP(obj_wall.modelMatrix);
		obj_wall.drawWithTexture(GL_TRIANGLES, shader);
		obj_ground.drawWithTexture(GL_TRIANGLES, shader);
		obj_skybox.drawWithTexture(GL_TRIANGLES, shader);

		shader.calcMVP(obj_target.modelMatrix);
		obj_target.drawWithTexture(GL_TRIANGLES, shader);

		obj_snow.step();
		if (obj_snow.indexed_normals.size() > 0)
			obj_snow.drawWithTexture(GL_TRIANGLES, shader);

//		shader.calcMVP(obj_spongebob.ModelMatrix);
//		obj_spongebob.drawWithTexture(GL_TRIANGLES, shader);
 
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
	srand(time(0));

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
	int width, height;
	glfwGetWindowSize(window, &width, &height);
	TwWindowSize(width, height);
	TwBar * GUI = TwNewBar("Quick Menu");
	{
		int size[] = { 320, 400};
		TwSetParam(GUI, NULL, "size", TW_PARAM_INT32, 2, size);
		int width = 180; // pixels
		TwSetParam(GUI, NULL, "valueswidth", TW_PARAM_INT32, 1, &width);
	}
	// FPS
	TwAddVarRO(GUI, "FPS", TW_TYPE_DOUBLE, &gFPS, "group='FPS'");
	// display mode
	TwAddVarRO(GUI, "Position", TW_TYPE_DIR3F, &gCamPosition, "group='Object Information'");

	// Callback functions
	glfwSetMouseButtonCallback(window, (GLFWmousebuttonfun)TwEventMouseButtonGLFW); // - Directly redirect GLFW mouse button events to AntTweakBar
	glfwSetCharCallback(window, (GLFWcharfun)TwEventCharGLFW);                      // - Directly redirect GLFW char events to AntTweakBar
	glfwSetMouseButtonCallback(window, (GLFWmousebuttonfun)MouseHandler); 
	glfwSetCursorPosCallback(window, (GLFWcursorposfun)CursorHandler);   
	glfwSetScrollCallback(window, (GLFWscrollfun)ScrollHandler);  
	glfwSetKeyCallback(window, (GLFWkeyfun)KeyHandler); 

	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	glfwSetCursorPos(window, width/2, height/2);
	glClearColor(47.0/256.0, 56.0/256.0, 85.0/256.0, 0.0f);
	return true;
}