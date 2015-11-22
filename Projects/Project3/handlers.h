#pragma once

#ifndef __HANDLERS__
#define __HANDLERS__

#define GLEW_STATIC
#include <GL/glew.h>
#include <glfw3.h>

#define TW_STATIC 
#include <AntTweakBar.h>
#include <glm/glm.hpp>



void MouseHandler(GLFWwindow* window, int button, int action, int mods);
void CursorHandler(GLFWwindow* window, double xpos, double ypos);
void ScrollHandler(GLFWwindow* window, double xoffset, double yoffset);
void KeyHandler(GLFWwindow* window, int key, int scancode, int action, int mods);
void movementCalc(glm::mat4 &ProjectionMatrix, glm::mat4 &ViewMatrix);

void FPS_calc(GLfloat FPS);
void FPS_init();
static double lastTime;
static double lastFrameTime;
static int nbFrames;

#endif
