#include "CShader.h"
#include "../common/shader.hpp"
#include "handlers.h"
#include <glm/glm.hpp>

CShader::CShader() {
	ModelMatrix = glm::mat4(1.0);
}

CShader::CShader(const char * vertex_file_path,const char * fragment_file_path) {
	ModelMatrix = glm::mat4(1.0);

	programID = LoadShaders(vertex_file_path, fragment_file_path);
	MatrixID = glGetUniformLocation(programID, "MVP");
	ViewMatrixID = glGetUniformLocation(programID, "V");
	ModelMatrixID = glGetUniformLocation(programID, "M");
 
	vertexPosition_modelspaceID = glGetAttribLocation(programID, "vertexPosition_modelspace");
	vertexUVID = glGetAttribLocation(programID, "vertexUV");
	vertexNormal_modelspaceID = glGetAttribLocation(programID, "vertexNormal_modelspace");
}

CShader::~CShader() {
	glDeleteProgram(programID);
}

void CShader::useProgram() {
	glUseProgram(programID);
}

void CShader::calcMVP(glm::mat4 &model) {
	ModelMatrix = model;
	this->calcMVP();
}

void CShader::calcMVP() {
	movementCalc(ProjectionMatrix, ViewMatrix);
	MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
	glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
	glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);
}
