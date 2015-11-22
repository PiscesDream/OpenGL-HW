#include "handlers.h"

#include "parameters.h"
#include "CMap.h"
#include <glm/gtc/matrix_transform.hpp>

void MouseHandler(GLFWwindow* window, int button, int action, int mods) {
	TwEventMouseButtonGLFW(window, button, action, mods);
/*	if (action == GLFW_PRESS && button == GLFW_MOUSE_BUTTON_LEFT) {
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		gLastx = xpos;
		gLasty = ypos;
	}*/
}
void CursorHandler(GLFWwindow* window, double xpos, double ypos) {
	TwEventMousePosGLFW(window, xpos, ypos);
}
void ScrollHandler(GLFWwindow* window, double xoffset, double yoffset) {
	TwEventMouseWheelGLFW(window, xoffset, yoffset);
/*	gScaleAlpha += 0.05*yoffset;
	if (gScaleAlpha < 0.01) gScaleAlpha = 0.01f;
	printf("Scale=%.3f xoffset=%.3f\n", gScaleAlpha, yoffset);*/
}
void KeyHandler(GLFWwindow* window, int key, int scancode, int action, int mods) {
	TwEventKeyGLFW(key, action);
  	/*double scale = 0.3f;
	if (action == GLFW_PRESS)
		switch (key) {
		case GLFW_KEY_W: {
			gCamPosition.x += scale;
			break;
		}
		case GLFW_KEY_S: {
			gCamPosition.x -= scale;
			break;
		}
		case GLFW_KEY_A: {
			gCamPosition.y += scale;
			break;
		}
		case GLFW_KEY_D: {
			gCamPosition.y -= scale;
			break;
		}
		default: {
		}
		} */
}

extern CMap map;
extern GLFWwindow *window;
extern glm::vec3 gCamEularAngle;
extern glm::vec3 gCamSpeed;
extern glm::vec3 gCamPosition;
extern GLfloat gCamFov;

void movementCalc(glm::mat4 &ProjectionMatrix, glm::mat4 &ViewMatrix){
	static double lastTime = glfwGetTime();
	double currentTime = glfwGetTime();
	float deltaTime = float(currentTime - lastTime);

	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);
	glfwSetCursorPos(window, 1024/2, 768/2);
	gCamEularAngle.x += Params::mouseSpeed * float(1024/2-xpos);
	gCamEularAngle.y += Params::mouseSpeed * float(768/2-ypos);

	glm::vec3 direction(
		cos(gCamEularAngle.x),
		sin(gCamEularAngle.x),
		sin(gCamEularAngle.y)
	);
	glm::vec3 right = glm::vec3(
		cos(1.57f+gCamEularAngle.x),
		sin(1.57f+gCamEularAngle.x),
		0
	);
	
	if (glfwGetKey( window, GLFW_KEY_W) == GLFW_PRESS){
		gCamSpeed += direction * deltaTime * Params::accelerate;
	}
	if (glfwGetKey( window, GLFW_KEY_S) == GLFW_PRESS){
		gCamSpeed  -= direction * deltaTime * Params::accelerate;
	}
	if (glfwGetKey( window, GLFW_KEY_A) == GLFW_PRESS){
		gCamSpeed += right * deltaTime * Params::accelerate;
	}
	if (glfwGetKey( window, GLFW_KEY_D) == GLFW_PRESS){
		gCamSpeed -= right * deltaTime * Params::accelerate;
	}

	gCamSpeed.x *=  Params::fiction;
	gCamSpeed.y *= Params::fiction;


	{
		// wall collision
		gCamPosition += gCamSpeed;
		if (map.fetchPointType(gCamPosition) == WALL)
			gCamSpeed = -gCamSpeed * Params::boundMargin;
	}

	gCamPosition.z = Params::cam_height + map.fetchPointHeight(gCamPosition);


//lm::mat4 perspective(float fovy, float aspect, float zNear, float zFar);
	ProjectionMatrix = glm::perspective(gCamFov, 4.0f / 3.0f, 0.1f, 5000.0f);
	ViewMatrix       = glm::lookAt(
								gCamPosition,         
								gCamPosition+direction,
								glm::vec3(0,0,1)
						   );

	lastTime = currentTime;
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