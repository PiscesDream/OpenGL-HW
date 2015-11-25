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
	bool loaded;

	void load(const char *, CShader &);
	CTexture();
	~CTexture();
	void activateTexture();
};

#endif
