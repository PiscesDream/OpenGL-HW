#include "CTexture.h"
#include "../common/texture.hpp"

CTexture::CTexture() {
}

CTexture::CTexture(const char * filename, CShader &shader) {
	gTexture = loadBMP_custom(filename);
	TextureID  = glGetUniformLocation(shader.programID, "myTextureSampler");
}

CTexture::~CTexture() {
	glDeleteTextures(1, &TextureID);
}

void CTexture::activateTexture() {
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gTexture);
	glUniform1i(TextureID, 0);
}