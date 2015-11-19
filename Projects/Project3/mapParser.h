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
bool parseMap(char * filename, vector<glm::vec3> &vertices, vector<glm::vec2> &uvs, vector<glm::vec3> &normals);

#endif

