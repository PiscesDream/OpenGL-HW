#include "CMap.h"

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include "parameters.h"

#define __DEBUG__

using namespace std;
using glm::vec3;

inline int parseChar(char c) {
    for (register int i = 0; i < MAPTABLELEN; ++i) 
        if (c==MAPTABLE[i]) return i;
    return -1;
}

void AddSameNormalRect(CObj &obj, vec3 p1, vec3 p2, vec3 p3, vec3 p4, vec3 n) {
	obj.vertices.push_back(p1);
	obj.vertices.push_back(p2);
	obj.vertices.push_back(p3);
	obj.uvs.push_back(glm::vec2(0, 0));
	obj.uvs.push_back(glm::vec2(1, 0));
	obj.uvs.push_back(glm::vec2(0, 1));
	obj.normals.push_back(n);
	obj.normals.push_back(n);
	obj.normals.push_back(n);
	obj.vertices.push_back(p2);
	obj.vertices.push_back(p3);
	obj.vertices.push_back(p4);
	obj.uvs.push_back(glm::vec2(1, 0));
	obj.uvs.push_back(glm::vec2(0, 1));
	obj.uvs.push_back(glm::vec2(1, 1));
	obj.normals.push_back(n);
	obj.normals.push_back(n);
	obj.normals.push_back(n);
}


CMap::~CMap() {
};

CMap::CMap() {
}

float CMap::fetchPointHeight(glm::vec3 p) {
	int xind = int(float(p.x) / Params::cell_width),
		yind = int(float(p.y) / Params::cell_width);
	return maph[xind][yind];
}

int CMap::fetchPointType(glm::vec3 p) {
	int xind = int(float(p.x) / Params::cell_width),
		yind = int(float(p.y) / Params::cell_width);
	return map[xind][yind];
}


bool CMap::parseMap(const char * filename, CObj & obj) {
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
		}
	for (int k = 0; k < Params::hill_height; ++k) {
		int cx = rand() % (n + 2),
			cy = rand() % (m + 2);
		double maxd = sqrt(n*n + m*m);
		for (int i = 0; i <= n + 2; ++i)
			for (int j = 0; j <= m + 2; ++j) {
				double dist = sqrt((cx-i)*(cx-i)+(cy-j)*(cy-j));
				double added = float(rand()) / RAND_MAX;
				added = exp(-dist/10) * Params::hill_height;
				maph[i][j] += added;
			}
	}


	for (int i = 0; i <= n + 2; ++i)
		for (int j = 0; j <= m + 2; ++j)
			AddSameNormalRect(obj, Params::cell_width*vec3(i, j, 0) + vec3(0, 0, maph[i][j]),
				Params::cell_width*vec3(i - 1, j, 0) + vec3(0, 0, maph[i - 1][j]),
				Params::cell_width*vec3(i, j - 1, 0) + vec3(0, 0, maph[i][j - 1]),
				Params::cell_width*vec3(i - 1, j - 1, 0) + vec3(0, 0, maph[i - 1][j - 1]),
				vec3(0, 0, 1));
	{
		// smooth
		int kx[] = { -1, -1, -1, 0, /*0,*/ 0, 1, 1, 1 },
			ky[] = { -1, 0, 1, -1, /*0,*/ 1, -1, 0, 1 };
		for (int t = 0; t < Params::smooth_times; ++t) 
			for (int i = 1; i <= n + 1; ++i)
				for (int j = 1; j <= m + 1; ++j) {
					float acc = 0;
					for (int k = 0; k < 8; ++k)
						acc += maph[i - kx[k]][j - ky[k]];
					maph[i][j] = acc/8.0;
				}
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
					p1 += vec3(0, 0, this->fetchPointHeight(p1));
					glm::vec3 p2(center.x + (kxy[k][0][0]+kxy[k][2][0]) * r, 
								 center.y + (kxy[k][0][1]+kxy[k][2][1]) * r, 
								0); 
					p2 += vec3(0, 0, this->fetchPointHeight(p2));
					glm::vec3 p3 = p1; p3.z += Params::cell_height;
					glm::vec3 p4 = p2; p4.z += Params::cell_height;
					AddSameNormalRect(obj, p1, p2, p3, p4, glm::vec3(kxy[k][0][0], kxy[k][0][1], 0));
				}

				glm::vec3 hh = glm::vec3(0, 0, Params::cell_height);
				glm::vec3 p1 = glm::vec3(i, j, 0) * Params::cell_width+hh;
				glm::vec3 p2 = glm::vec3(i+1, j, 0) * Params::cell_width+hh;
				glm::vec3 p3 = glm::vec3(i, j+1, 0) * Params::cell_width+hh;
				glm::vec3 p4 = glm::vec3(i+1, j+1, 0) * Params::cell_width+hh;
				AddSameNormalRect(obj, p1, p2, p3, p4, glm::vec3(0, 0, 1));
			}
}

extern glm::vec3 gLightPosition, gCamPosition;
void CMap::loadMap(const char * mapFilename, CObj & obj) {
	printf("Loading map %s ...", mapFilename);
	parseMap(mapFilename, obj);

	obj.getIndex();
	obj.getBuffer();

	gLightPosition = glm::vec3(n / 2 * Params::cell_width, m / 2 * Params::cell_width, 6);
	gCamPosition = glm::vec3(sx* Params::cell_width, sy* Params::cell_width, 0);

	printf("vertices size: %d\n", obj.vertices.size());
	printf("indexed vertices size: %d\n", obj.indexed_vertices.size());
}