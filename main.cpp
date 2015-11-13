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
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/norm.hpp>
using namespace glm;
 
// Include AntTweakBar
#define TW_STATIC 
#include <AntTweakBar.h>
 
#include <dirent.h>

#include "common/shader.hpp"
#include "common/texture.hpp"
#include "common/controls.hpp"
#include "common/objloader.hpp"
#include "common/vboindexer.hpp"
#include "common/quaternion_utils.hpp"



// Customize code
vec3 gPosition( 0.0f, 0.0f, 0.0f);
quat gOrientation;
vec3 gEulerAngles(0.0f,0.0f,0.0f);
bool gPointModel = false;
bool gLineModel = false;
static vec3 gLightPosition(4.0f, 4.0f, 4.0f);
static double gLastx = 0.0, gLasty = 0.0;
static double gScaleAlpha = 1.0;
static GLfloat gPointSize(20.0);
static GLfloat gLineWidth(1.0);
static double gFPS = 0.0;
char gModelFilename[1000];

static void MouseHandler(GLFWwindow* window, int button, int action, int mods) {
	TwEventMouseButtonGLFW(window, button, action, mods);
	if (action == GLFW_PRESS && button == GLFW_MOUSE_BUTTON_LEFT) {
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		gLastx = xpos;
		gLasty = ypos;
	}
}
static void CursorHandler(GLFWwindow* window, double xpos, double ypos) {
	TwEventMousePosGLFW(window, xpos, ypos);
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
		double deltax = xpos - gLastx, deltay = ypos - gLasty;
		gLastx = xpos; gLasty = ypos;

		// Conversion from Euler angles (in radians) to Quaternion
		vec3 gEulerAnglesDelta(0.01*(deltay), 0.01*(deltax), 0);
  		//gEulerAngles += gEulerAnglesDelta;
		quat tmpOrient = quat(gEulerAnglesDelta);
		gOrientation = tmpOrient * gOrientation;

//		double k = sqrt(1 - pow(gOrientation.w, 2));
//		gEulerAngles = glm::vec3(gOrientation.x/k, gOrientation.y/k, gOrientation.z/k)+gEulerAnglesDelta;
//		gEulerAngles += rotate(gOrientation, gEulerAnglesDelta);

		printf("deltax=%.3f deltay=%.3f\n", deltax, deltay);
		printf("gEularAngle = %.3f %.3f %.3f\n", gEulerAngles.x, gEulerAngles.y, gEulerAngles.z);
		printf("gOrientation = %.3f %.3f %.3f %.3f\n", gOrientation.x, gOrientation.y, gOrientation.z, gOrientation.w);
	}
}
static void ScrollHandler(GLFWwindow* window, double xoffset, double yoffset) {
	TwEventMouseWheelGLFW(window, xoffset, yoffset);
	gScaleAlpha += 0.05*yoffset;
	if (gScaleAlpha < 0.01) gScaleAlpha = 0.01f;
	printf("Scale=%.3f xoffset=%.3f\n", gScaleAlpha, yoffset);
}
static void KeyHandler(GLFWwindow* window, int key, int scancode, int action, int mods) {
	TwEventKeyGLFW(key, action);
	double scale = 0.3f;
	if (action == GLFW_PRESS)
		switch (key) {
		case GLFW_KEY_UP: {
			gPosition.y += scale;
			break;
		}
		case GLFW_KEY_DOWN: {
			gPosition.y -= scale;
			break;
		}
		case GLFW_KEY_LEFT: {
			gPosition.x -= scale;
			break;
		}
		case GLFW_KEY_RIGHT: {
			gPosition.x += scale;
			break;
		}
		default: {
		}
		}
}

std::vector<glm::vec3> vertices;
std::vector<glm::vec2> uvs;
std::vector<glm::vec3> normals;

std::vector<unsigned short> indices;
std::vector<glm::vec3> indexed_vertices;
std::vector<glm::vec2> indexed_uvs;
std::vector<glm::vec3> indexed_normals;

GLuint vertexbuffer;
GLuint uvbuffer;
GLuint normalbuffer;
GLuint elementbuffer;

void loadModel(char * modelFilename) {
	vertices.clear();
	uvs.clear();
	normals.clear();

	indices.clear();
	indexed_vertices.clear();
	indexed_uvs.clear();
	indexed_normals.clear();

	bool res = loadOBJ(modelFilename, vertices, uvs, normals);

	indexVBO(vertices, uvs, normals, indices, indexed_vertices, indexed_uvs, indexed_normals);
 
	// Load it into a VBO
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, indexed_vertices.size() * sizeof(glm::vec3), &indexed_vertices[0], GL_STATIC_DRAW);
 
	glGenBuffers(1, &uvbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glBufferData(GL_ARRAY_BUFFER, indexed_uvs.size() * sizeof(glm::vec2), &indexed_uvs[0], GL_STATIC_DRAW);
 
	glGenBuffers(1, &normalbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
	glBufferData(GL_ARRAY_BUFFER, indexed_normals.size() * sizeof(glm::vec3), &indexed_normals[0], GL_STATIC_DRAW);
 
	// Generate a buffer for the indices as well
	glGenBuffers(1, &elementbuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned short), &indices[0] , GL_STATIC_DRAW); 
}

char gModels[10][1000];
#define __createModelCallBack(ind)\
	void __modelcallback##ind (void*clientdata) {\
		loadModel(gModels[ind]);\
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
GLuint gTexture;
char gTextures[10][1000];
#define __createTextureCallBack(ind)\
	void __texturecallback##ind (void*clientdata) {\
		gTexture=loadBMP_custom(gTextures[ind]);\
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

	// Create and compile our GLSL program from the shaders
	GLuint programID = LoadShaders( "StandardShading.vertexshader", "StandardShading.fragmentshader" );
 
	// Get a handle for our "MVP" uniform
	GLuint MatrixID = glGetUniformLocation(programID, "MVP");
	GLuint ViewMatrixID = glGetUniformLocation(programID, "V");
	GLuint ModelMatrixID = glGetUniformLocation(programID, "M");
 
	// Get a handle for our buffers
	GLuint vertexPosition_modelspaceID = glGetAttribLocation(programID, "vertexPosition_modelspace");
	GLuint vertexUVID = glGetAttribLocation(programID, "vertexUV");
	GLuint vertexNormal_modelspaceID = glGetAttribLocation(programID, "vertexNormal_modelspace");
 
	// Load the texture
	//gTexture = loadBMP_custom(".\\Textures\\rock0.bmp");
	//GLuint Texture = loadDDS("uvmap.DDS");
	
	// Get a handle for our "myTextureSampler" uniform
	GLuint TextureID  = glGetUniformLocation(programID, "myTextureSampler");
 
	// Load Model
	loadModel(gModelFilename);

	// Get a handle for our "LightPosition" uniform
	glUseProgram(programID);
	GLuint LightID = glGetUniformLocation(programID, "LightPosition_worldspace");
 
	// For speed computation
	double lastTime = glfwGetTime();
	double lastFrameTime = lastTime;
	int nbFrames = 0;
	 
	do{
		// Measure speed
		double currentTime = glfwGetTime();
		float deltaTime = (float)(currentTime - lastFrameTime); 
		lastFrameTime = currentTime;
		nbFrames++;
		if ( currentTime - lastTime >= 1.0 ){ // If last prinf() was more than 1sec ago
			gFPS = double(nbFrames) / 1.0;
			nbFrames = 0;
			lastTime += 1.0;
		}
 
		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
 
		// Use our shader
		glUseProgram(programID);
 
		glm::mat4 ProjectionMatrix = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.0f);
		glm::mat4 ViewMatrix = glm::lookAt(
			glm::vec3( 0, 0, 10),
			glm::vec3( 0, 0, 0 ),
			glm::vec3( 0, 1, 0 ) 
		);
 
		// Bind our texture in Texture Unit 0
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, gTexture);
		// Set our "myTextureSampler" sampler to user Texture Unit 0
		glUniform1i(TextureID, 0);
 
		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glVertexAttribPointer(
			vertexPosition_modelspaceID,  // The attribute we want to configure
			3,                            // size
			GL_FLOAT,                     // type
			GL_FALSE,                     // normalized?
			0,                            // stride
			(void*)0                      // array buffer offset
		);
 
		// 2nd attribute buffer : UVs
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
		glVertexAttribPointer(
			vertexUVID,                   // The attribute we want to configure
			2,                            // size : U+V => 2
			GL_FLOAT,                     // type
			GL_FALSE,                     // normalized?
			0,                            // stride
			(void*)0                      // array buffer offset
		);
 
		// 3rd attribute buffer : normals
		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
		glVertexAttribPointer(
			vertexNormal_modelspaceID,    // The attribute we want to configure
			3,                            // size
			GL_FLOAT,                     // type
			GL_FALSE,                     // normalized?
			0,                            // stride
			(void*)0                      // array buffer offset
		);
 
		// Index buffer
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
 
		glm::vec3 lightPos = gLightPosition;
		glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);
 

		{ // Quaternion
			glm::mat4 RotationMatrix = mat4_cast(gOrientation);
			glm::mat4 TranslationMatrix = translate(mat4(), gPosition); // A bit to the right
			glm::mat4 ScalingMatrix = scale(mat4(), vec3(gScaleAlpha));
			glm::mat4 ModelMatrix = TranslationMatrix * RotationMatrix * ScalingMatrix;
 
			glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
 
			// Send our transformation to the currently bound shader, 
			// in the "MVP" uniform
			glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
			glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
			glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);
 
 
			if (gPointModel||gLineModel) {
				if (gPointModel) {
					glEnable(GL_PROGRAM_POINT_SIZE);
					glPointSize(gPointSize);
					glDrawElements(
						GL_POINTS,      // mode
						indices.size(),    // count
						GL_UNSIGNED_SHORT,   // type
						(void*)0           // element array buffer offset
					);
				}
				if (gLineModel) {
					glEnable(GL_LINE_WIDTH);
					glLineWidth(gLineWidth);
					glDrawElements(
						GL_LINES,      // mode
						indices.size(),    // count
						GL_UNSIGNED_SHORT,   // type
						(void*)0           // element array buffer offset
					);
				}
			}
			else
				glDrawElements(
					GL_TRIANGLES,      // mode
					indices.size(),    // count
					GL_UNSIGNED_SHORT,   // type
					(void*)0           // element array buffer offset
				);
		}
 
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);
 
		// Draw GUI
		TwDraw();
 
		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
 
	} // Check if the ESC key was pressed or the window was closed
	while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
		   glfwWindowShouldClose(window) == 0 );
 
	// Cleanup VBO and shader
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &uvbuffer);
	glDeleteBuffers(1, &normalbuffer);
	glDeleteBuffers(1, &elementbuffer);
	glDeleteProgram(programID);
	glDeleteTextures(1, &TextureID);
 
	// Close GUI and OpenGL window, and terminate GLFW
	TwTerminate();
	glfwTerminate();
 
	return 0;
}