#pragma once


#ifndef __CSHADER__
#define __CSHADER__

#include "GL/glew.h"
#include <glm/glm.hpp>

class CShader {
public:
	GLuint programID;
 
	GLuint MatrixID;
	GLuint ViewMatrixID;
	GLuint ModelMatrixID;
 
	GLuint vertexPosition_modelspaceID;
	GLuint vertexUVID;
	GLuint vertexNormal_modelspaceID;
	GLuint lightPowerID;

	CShader();
	CShader::CShader(const char * vertex_file_path, const char * fragment_file_path);
	~CShader();
	void CShader::load(const char * vertex_file_path, const char * fragment_file_path);
	void useProgram();
	void calcMVP();
	void calcMVP(glm::mat4 &model);
	void setLight(float ligthPower);

	glm::mat4 ProjectionMatrix;
	glm::mat4 ViewMatrix;
	glm::mat4 ModelMatrix; // of the whole world
	glm::mat4 MVP;
};
 

#endif
