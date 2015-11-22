#pragma once

#ifndef _MAP_PARSER_HPP_
#define _MAP_PARSER_HPP_

#include <vector>
#include <glm/glm.hpp>
#include "CObj.h"
#include "CTexture.h"

const int MAXN = 500;
const char MAPTABLE[] = ".#";
const char MAPTABLELEN = 2;
enum {
	GROUND, WALL
};

using std::vector;

class CMap{
public:
	int n, m, sx, sy, ex, ey;
    float w, l, h; 
    int map[MAXN+2][MAXN+2] = {0};
	float maph[MAXN + 2][MAXN + 2];

	CMap();
	~CMap();
	bool parseMap(const char * filename, CObj & obj);
	void loadMap(const char * filename, CObj & obj);

	int fetchPointType(glm::vec3 p);
	float fetchPointHeight(glm::vec3 p);
};

#endif

