#include "CTexture.h"
#include "../common/texture.hpp"

CTexture::CTexture() {
	loaded = false;
}

void CTexture::load(const char * filename, CShader &shader) {
		if (strstr(filename, "DDS")!=0)
			gTexture=loadDDS(filename);
		else
			gTexture=loadBMP_custom(filename);
		loaded = true;
	TextureID  = glGetUniformLocation(shader.programID, "myTextureSampler");
}

CTexture::~CTexture() {
	glDeleteTextures(1, &TextureID);
}

void CTexture::activateTexture() {
	glUniform1i(TextureID, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gTexture);
}