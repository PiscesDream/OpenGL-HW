#pragma once

#ifndef __CTEXTURE__
#define __CTEXTURE__

#define GLEW_STATIC
#include <GL/glew.h>
#include "CShader.h"

class CTexture {
public:
	GLuint gTexture; 
	GLuint TextureID;

	CTexture();
	CTexture(const char *, CShader &);
	~CTexture();
	void activateTexture();
};

#endif
