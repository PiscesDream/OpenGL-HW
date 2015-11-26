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
using namespace glm;
 
// Include AntTweakBar
#define TW_STATIC 
#include <AntTweakBar.h>
 
#include <dirent.h>

#include "../common/objloader.hpp"
#include "../common/controls.hpp"
#include "../common/quaternion_utils.hpp"
#include "../common/shader.hpp"
#include "../common/tangentspace.hpp"
#include "../common/text2D.hpp"
#include "../common/texture.hpp"
#include "../common/vboindexer.hpp"

#include "handlers.h"
#include <CObj.h>
#include <CShader.h>
#include <CTexture.h>

// Customize code
vec3 gPosition( 0.0f, 0.0f, 0.0f);
quat gOrientation;
vec3 gEulerAngles(0.0f,0.0f,0.0f);
bool gPointModel = false;
bool gLineModel = false;
vec3 gLightPosition(4.0f, 4.0f, 4.0f);
double gLastx = 0.0, gLasty = 0.0;
double gScaleAlpha = 1.0;
GLfloat gPointSize(20.0);
GLfloat gLineWidth(1.0);
double gFPS = 0.0;
char gModelFilename[1000];


int init();
CShader shader;
CObj obj;
char gModels[10][1000];
#define __createModelCallBack(ind)\
	void __modelcallback##ind (void*clientdata) {\
		obj.loadObj(gModels[ind]);\
		strcpy(gModelFilename, gModels[ind]);\
	};
__createModelCallBack(0)
__createModelCallBack(1)
__createModelCallBack(2)
__createModelCallBack(3)
__createModelCallBack(4)
__createModelCallBack(5)
__createModelCallBack(6)
__createModelCallBack(7)
__createModelCallBack(8)
__createModelCallBack(9)
TwButtonCallback gModelCallback[10] = { 
	__modelcallback0,
	__modelcallback1,
	__modelcallback2,
	__modelcallback3,
	__modelcallback4,
	__modelcallback5,
	__modelcallback6,
	__modelcallback7,
	__modelcallback8,
	__modelcallback9
};


char gTextureFilename[1000];
char gTextures[10][1000];
#define __createTextureCallBack(ind)\
	void __texturecallback##ind (void*clientdata) {\
		obj.loadTexture(gTextures[ind], shader);\
		strcpy(gTextureFilename, gTextures[ind]);\
	};
__createTextureCallBack(0)
__createTextureCallBack(1)
__createTextureCallBack(2)
__createTextureCallBack(3)
__createTextureCallBack(4)
__createTextureCallBack(5)
__createTextureCallBack(6)
__createTextureCallBack(7)
__createTextureCallBack(8)
__createTextureCallBack(9)
TwButtonCallback gTextureCallback[10] = { 
	__texturecallback0,
	__texturecallback1,
	__texturecallback2,
	__texturecallback3,
	__texturecallback4,
	__texturecallback5,
	__texturecallback6,
	__texturecallback7,
	__texturecallback8,
	__texturecallback9
};

int main(void)
{
	if (init() == -1) 
		return EXIT_FAILURE;


	shader.load( "StandardShading.vertexshader", "StandardShading.fragmentshader" );
	shader.setLight(100.0f);
	GLuint programID = shader.programID;
 
	obj.loadObj(gModels[0]);
	obj.loadTexture(gTextures[0], shader);

	glUseProgram(programID);
	GLuint LightID = glGetUniformLocation(programID, "LightPosition_worldspace");
 
	FPS_init();
	do{
		FPS_calc(gFPS);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
 
		glUseProgram(programID);

		glm::vec3 lightPos = gLightPosition;
		glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);
 
		glm::mat4 RotationMatrix = mat4_cast(gOrientation);
		glm::mat4 TranslationMatrix = translate(mat4(), gPosition); 
		glm::mat4 ScalingMatrix = scale(mat4(), vec3(gScaleAlpha));
		glm::mat4 ModelMatrix = TranslationMatrix * RotationMatrix * ScalingMatrix;

		shader.calcMVP(ModelMatrix);
		if (gPointModel||gLineModel) {
			if (gPointModel) {
				glEnable(GL_PROGRAM_POINT_SIZE);
				glPointSize(gPointSize);
				obj.drawWithTexture(GL_POINTS, shader);
			}
			if (gLineModel) {
				glEnable(GL_LINE_WIDTH);
				glLineWidth(gLineWidth);
				obj.drawWithTexture(GL_LINES, shader);
			}
		}
		else
			obj.drawWithTexture(GL_TRIANGLES, shader);
 
		TwDraw();
		glfwSwapBuffers(window);
		glfwPollEvents();
 
	} 
	while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS && glfwWindowShouldClose(window) == 0 );
 

	TwTerminate();
	glfwTerminate();
 
	return 0;
}

int init() {



	// Init GLFW
	if (!glfwInit()) {
		fprintf(stderr, "Failed to initialize GLFW\n");
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4); // anti-alising
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(1024, 768, "Project2", NULL, NULL);
	if (window == NULL) {
		fprintf(stderr, "Failed to open GLFW window. Assure your display driver is compatible with GLFW3.3.\n");
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		return -1;
	}

	// Initialize the GUI
	TwInit(TW_OPENGL_CORE, NULL);
	int width, height;
	glfwGetWindowSize(window, &width, &height);
	TwWindowSize(width, height);
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
	// Models
	TwAddVarRO(GUI, "Current Model", TW_TYPE_CSSTRING(sizeof(gModelFilename)), gModelFilename, "group=Models label='Current model'");
	{
		DIR *dir;
		struct dirent *ent;
		dir = opendir(".\\Models\\");
		int cnt = 0;
		if (dir != NULL) {
			while ((ent = readdir(dir)) != NULL) {
				if (ent->d_type == DT_REG) {
					if (cnt >= 10) {
						printf("Can only read 10 models at most");
						continue;
					}
					strcpy(gModelFilename, ".\\Models\\");
					strcat(gModelFilename, ent->d_name);
					strcpy(gModels[cnt], gModelFilename);
					TwAddButton(GUI, ent->d_name, (TwButtonCallback)gModelCallback[cnt], NULL, "group=Models");
					cnt++;
				}
			}
			closedir(dir);
		}
		else {
			printf("Cannot find Directory Models.\n");
			return -1;
		}
	}
	// Textures 
	TwAddVarRO(GUI, "Current Texture", TW_TYPE_CSSTRING(sizeof(gTextureFilename)), gTextureFilename, "group=Textures label='Current texture'");
	{
		DIR *dir;
		struct dirent *ent;
		dir = opendir(".\\Textures\\");
		int cnt = 0;
		if (dir != NULL) {
			while ((ent = readdir(dir)) != NULL) {
				if (ent->d_type == DT_REG) {
					if (cnt >= 10) {
						printf("Can only read 10 texture at most");
						continue;
					}
					strcpy(gTextureFilename, ".\\Textures\\");
					strcat(gTextureFilename, ent->d_name);
					strcpy(gTextures[cnt], gTextureFilename);
					TwAddButton(GUI, ent->d_name, (TwButtonCallback)gTextureCallback[cnt], NULL, "group=Textures");
					cnt++;
				}
			}
			closedir(dir);
		}
		else {
			printf("Cannot find Directory Models.\n");
			return -1;
		}
	}


	// Callback functions
	glfwSetMouseButtonCallback(window, (GLFWmousebuttonfun)TwEventMouseButtonGLFW); // - Directly redirect GLFW mouse button events to AntTweakBar
	glfwSetCharCallback(window, (GLFWcharfun)TwEventCharGLFW);                      // - Directly redirect GLFW char events to AntTweakBar
	glfwSetMouseButtonCallback(window, (GLFWmousebuttonfun)MouseHandler); 
	glfwSetCursorPosCallback(window, (GLFWcursorposfun)CursorHandler);   
	glfwSetScrollCallback(window, (GLFWscrollfun)ScrollHandler);  
	glfwSetKeyCallback(window, (GLFWkeyfun)KeyHandler); 

	// keybroad
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	// cursor
	glfwSetCursorPos(window, 1024/2, 768/2);
	// Light gray background
	glClearColor(0.4f, 0.4f, 0.4f, 0.0f);
	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS); 
	// Cull triangles which normal is not towards the camera
	//glEnable(GL_CULL_FACE);
}