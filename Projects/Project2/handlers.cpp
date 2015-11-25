#include "handlers.h"


using glm::quat;
using glm::vec3;


extern vec3 gPosition;
extern quat gOrientation;
extern vec3 gEulerAngles;
extern bool gPointModel;
extern bool gLineModel;
extern vec3 gLightPosition;
extern double gLastx, gLasty;
extern double gScaleAlpha;
extern GLfloat gPointSize;
extern GLfloat gLineWidth;
extern double gFPS;
extern char gModelFilename[1000];

void MouseHandler(GLFWwindow* window, int button, int action, int mods) {
	TwEventMouseButtonGLFW(window, button, action, mods);
	if (action == GLFW_PRESS && button == GLFW_MOUSE_BUTTON_LEFT) {
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		gLastx = xpos;
		gLasty = ypos;
	}
}
void CursorHandler(GLFWwindow* window, double xpos, double ypos) {
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
void ScrollHandler(GLFWwindow* window, double xoffset, double yoffset) {
	TwEventMouseWheelGLFW(window, xoffset, yoffset);
	gScaleAlpha += 0.05*yoffset;
	if (gScaleAlpha < 0.01) gScaleAlpha = 0.01f;
	printf("Scale=%.3f xoffset=%.3f\n", gScaleAlpha, yoffset);
}
void KeyHandler(GLFWwindow* window, int key, int scancode, int action, int mods) {
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


void FPS_init() {	
	lastTime = glfwGetTime();
	lastFrameTime = lastTime;
	nbFrames = 0;
}

void FPS_calc(GLfloat FPS) {
	double currentTime = glfwGetTime();
	float deltaTime = (float)(currentTime - lastFrameTime); 
	lastFrameTime = currentTime;
	nbFrames++;
	if ( currentTime - lastTime >= 1.0 ){ // If last prinf() was more than 1sec ago
		FPS = double(nbFrames) / 1.0;
		nbFrames = 0;
		lastTime += 1.0;
	}
}