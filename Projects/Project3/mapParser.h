#pragma once

#ifndef _MAP_PARSER_HPP_
#define _MAP_PARSER_HPP_

#include <vector>
#include <glm/glm.hpp>

const int MAXN = 100;
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
	double maph[MAXN + 2][MAXN + 2];

	CMap();
	~CMap();
	bool parseMap(char * filename, vector<glm::vec3> &vertices, vector<glm::vec2> &uvs, vector<glm::vec3> &normals);
};

#endif

