#pragma once

#ifndef __CSKYBOX__
#define __CSKYBOX__

#include "CObj.h"
#include "CShader.h"

class CSkybox {
public:
	float x1, y1, x2, y2, height;
	CObj obj;

	CSkybox(float x1, float y1, float x2, float y2, float height);
	CSkybox();
	~CSkybox();
};

#endif
