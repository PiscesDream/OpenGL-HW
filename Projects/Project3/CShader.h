#pragma once


#ifndef __CSHADER__
#define __CSHADER__

#include "GL/glew.h"

class CShader {
public:
	GLuint programID;
 
	GLuint MatrixID;
	GLuint ViewMatrixID;
	GLuint ModelMatrixID;
 
	GLuint vertexPosition_modelspaceID;
	GLuint vertexUVID;
	GLuint vertexNormal_modelspaceID;

	CShader();
	CShader::CShader(const char * vertex_file_path, const char * fragment_file_path);
	~CShader();
	void useProgram();
	void calcMVP();
};
 

#endif
