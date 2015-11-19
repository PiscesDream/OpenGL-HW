#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include "mapParser.h"
#include "parameters.h"

#define __DEBUG__

using namespace std;

inline int parseChar(char c) {
    for (register int i = 0; i < MAPTABLELEN; ++i) 
        if (c==MAPTABLE[i]) return i;
    return -1;
}

#define AddGroudPoint(i,j,dx,dy) \
	vertices.push_back(Params::cell_width*glm::vec3(i-dx, j-dy, maph[i-dx][j-dy]));\
	uvs.push_back(glm::vec2(dx,dy));\
	normals.push_back(glm::vec3(0, 0, 1));
#define AddGround(i,j)\
	AddGroudPoint(i, j, 0, 0);\
	AddGroudPoint(i, j, 1, 0);\
	AddGroudPoint(i, j, 1, 1);\
	AddGroudPoint(i, j, 0, 0);\
	AddGroudPoint(i, j, 0, 1);\
	AddGroudPoint(i, j, 1, 1);

#define AddSameNormalRect(p1, p2, p3, p4, n)\
	vertices.push_back(p1);\
	vertices.push_back(p2);\
	vertices.push_back(p3);\
	uvs.push_back(glm::vec2(0, 0));\
	uvs.push_back(glm::vec2(1, 0));\
	uvs.push_back(glm::vec2(0, 1));\
	normals.push_back(n);\
	normals.push_back(n);\
	normals.push_back(n);\
	vertices.push_back(p2);\
	vertices.push_back(p3);\
	vertices.push_back(p4);\
	uvs.push_back(glm::vec2(1, 0));\
	uvs.push_back(glm::vec2(0, 1));\
	uvs.push_back(glm::vec2(1, 1));\
	normals.push_back(n);\
	normals.push_back(n);\
	normals.push_back(n);


CMap::~CMap() {
};

CMap::CMap() {
}


bool CMap::parseMap(char * filename, vector<glm::vec3> &vertices, vector<glm::vec2> &uvs, vector<glm::vec3> &normals) {
    ifstream file(filename);
	if (!file.is_open()) return false;
	
    // start to read file
    // exception handler should be added
    file >> n >> m
		 >> sx >> sy
		 >> ex >> ey
		 >> w >> l >> h; 
    // read map
    for (int i = 1; i <= n; ++i) {
        string line;
        file >> line; 
        for (int j = 1; j <= m; ++j) 
            map[i][j] = parseChar(line[j-1]);
    }
    // fill walls
    for (int i = 0; i <= n+1; ++i)
        map[i][0] = map[i][m+1] = WALL;
    for (int i = 0; i <= m+1; ++i)
        map[0][i] = map[n+1][i] = WALL;
    // display the map
    #ifdef __DEBUG__
    for (int i = 0; i <= n+1; ++i, cout << endl) 
        for (int j = 0; j <= m+1; ++j, cout << ' ') 
            cout << map[i][j];
    #endif


	// floor
	for (int i = 0; i <= n + 2; ++i)
		for (int j = 0; j <= m + 2; ++j) {
			maph[i][j] = Params::ground_max_height*rand() / RAND_MAX;
			if (i == 0) continue;
			if (j == 0) continue;

			AddGround(i, j);
		}

	// Walls 
	const float kxy[4][3][2] = { {{1, 0}, {0, -1}, {0, 1}},
								{{-1, 0}, {0, -1}, {0, 1}},
								{{0, 1}, {1, 0}, {-1, 0}},
								{{0, -1}, {1, 0}, {-1, 0}} };
	for (int i = 0; i <= n + 1; ++i)
		for (int j = 0; j <= m + 1; ++j) 
			if (map[i][j] == WALL) {
				float r = 0.5 * Params::cell_width;
				glm::vec3 center = (glm::vec3(i, j, 0) * 2.0f + 1.0f) * r;
				for (int k = 0; k < 4; ++k) {
					glm::vec3 p1(center.x + (kxy[k][0][0]+kxy[k][1][0]) * r, 
								 center.y + (kxy[k][0][1]+kxy[k][1][1]) * r, 
								0); 
					glm::vec3 p2(center.x + (kxy[k][0][0]+kxy[k][2][0]) * r, 
								 center.y + (kxy[k][0][1]+kxy[k][2][1]) * r, 
								0); 
					glm::vec3 p3 = p1; p3.z += Params::cell_height;
					glm::vec3 p4 = p2; p4.z += Params::cell_height;
					AddSameNormalRect(p1, p2, p3, p4, glm::vec3(kxy[k][0][0], kxy[k][0][1], 0));
				}

				glm::vec3 hh = glm::vec3(0, 0, Params::cell_height);
				glm::vec3 p1 = glm::vec3(i, j, 0) * Params::cell_width+hh;
				glm::vec3 p2 = glm::vec3(i+1, j, 0) * Params::cell_width+hh;
				glm::vec3 p3 = glm::vec3(i, j+1, 0) * Params::cell_width+hh;
				glm::vec3 p4 = glm::vec3(i+1, j+1, 0) * Params::cell_width+hh;
				AddSameNormalRect(p1, p2, p3, p4, glm::vec3(0, 0, 1));
			}
}

