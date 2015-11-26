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
	int xind1 = ceil(float(p.x) / Params::cell_width),
		xind2 = floor(float(p.x) / Params::cell_width),
		yind1 = ceil(float(p.y) / Params::cell_width),
		yind2 = floor(float(p.y) / Params::cell_width);
	return 0.25*(maph[xind1][yind1] +maph[xind1][yind2] +maph[xind2][yind1] +maph[xind2][yind2]);
}

int CMap::fetchPointType(glm::vec3 p) {
	int xind = int(float(p.x) / Params::cell_width),
		yind = int(float(p.y) / Params::cell_width);
	return map[xind][yind];
}
extern glm::vec3 gLightPosition, gCamPosition;
void CMap::render(CObj & wobj, CObj & gobj) {
    // fill walls
    for (int i = 0; i <= n+1; ++i)
        map[i][0] = map[i][m+1] = WALL;
    for (int i = 0; i <= m+1; ++i)
        map[0][i] = map[n+1][i] = WALL;

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
				added = exp(-dist/Params::hill_norm) * Params::hill_height;
				maph[i][j] += added;
			}
	}
	for (int i = 0; i <= n + 2; ++i)
		for (int j = 0; j <= m + 2; ++j)
			AddSameNormalRect(gobj, Params::cell_width*vec3(i, j, 0) + vec3(0, 0, maph[i][j]),
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
					AddSameNormalRect(wobj, p1, p2, p3, p4, glm::vec3(kxy[k][0][0], kxy[k][0][1], 0));
				}

				glm::vec3 hh = glm::vec3(0, 0, Params::cell_height);
				glm::vec3 p1 = glm::vec3(i, j, 0) * Params::cell_width+hh;
				glm::vec3 p2 = glm::vec3(i+1, j, 0) * Params::cell_width+hh;
				glm::vec3 p3 = glm::vec3(i, j+1, 0) * Params::cell_width+hh;
				glm::vec3 p4 = glm::vec3(i+1, j+1, 0) * Params::cell_width+hh;
				AddSameNormalRect(wobj, p1, p2, p3, p4, glm::vec3(0, 0, 1));
			}

	//
	wobj.getIndex();
	wobj.getBuffer();

	gobj.getIndex();
	gobj.getBuffer();

	gLightPosition = glm::vec3(n / 2 * Params::cell_width, m / 2 * Params::cell_width, 6);
	gCamPosition = glm::vec3(sx* Params::cell_width, sy* Params::cell_width, 0);
}


void CMap::print() {
	printf("\n<Map: size=(%d * %d), start=(%d * %d), end=(%d * %d)\n", n, m, sx, sy, ex, ey);
	for (int i = 0; i <= n + 1; ++i, cout << endl)
		for (int j = 0; j <= m + 1; ++j, cout << ' ')
			printf("%c", MAPTABLE[map[i][j]]);
	printf("\n");
}

static int randint(int lower, int upper) {
	return int(round(lower + (upper - lower) * double(rand()) / RAND_MAX));
}

void CMap::split(int bottom, int left, int top, int right) {
	if (bottom + 1 >= top) return;
	if (left + 1 >= right) return;
	if (bottom + 1 == sx && sx + 1 == top) return;
	if (left + 1 == sy && sy + 1 == right) return;
	//printf("spliting %d %d %d %d ...\n", bottom, left, top, right);

	int wallx, wally, xentry, yentry;
	do { wallx = randint(bottom + 1, top - 1); } while (wallx == sx);
	do { wally = randint(left + 1, right - 1); } while (wally == sy);

	for (int i = bottom; i <= top; ++i)
		map[i][wally] = WALL;
	for (int i = left; i <= right; ++i)
		map[wallx][i] = WALL;

	map[wallx][randint(left, wally-1)] = GROUND;
	map[wallx][randint(wally+1, right)] = GROUND;
	map[randint(bottom, wallx-1)][wally] = GROUND;
	map[randint(wallx+1, top)][wally] = GROUND;

	//print();
	split(bottom, left, wallx - 1, wally - 1);
	split(wallx + 1, left, top, wally - 1);
	split(wallx + 1, wally + 1, top, right);
	split(bottom, wally + 1, wallx - 1, right);
	//printf("return %d %d %d %d ...\n", bottom, left, top, right);
}

void CMap::generateMap(int _n, int _m, float complexity) {
	n = _n;
	m = _m;
	sx = randint(1, n);
	sy = randint(1, m);
	ex = randint(1, n);
	ey = randint(1, m);

	split(1, 1, n, m);
}


bool CMap::parseMap(const char * filename) {
	printf("Loading map %s ...", filename);
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
}

