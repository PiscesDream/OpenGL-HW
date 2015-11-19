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

bool parseMap(char * filename, vector<glm::vec3> &vertices, vector<glm::vec2> &uvs, vector<glm::vec3> &normals) {
    ifstream file(filename);
	if (!file.is_open()) return false;
	
    // start to read file
    // exception handler should be added
    int n, m, sx, sy, ex, ey;
    float w, l, h; 
    int map[MAXN+2][MAXN+2] = {0};

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
	double maph[MAXN + 2][MAXN + 2];
	for (int i = 0; i <= n + 1; ++i)
		for (int j = 0; j <= m + 1; ++j) {
			maph[i][j] = Params::ground_max_height*rand() / RAND_MAX;
			if (i == 0) continue;
			if (j == 0) continue;

			vertices.push_back(glm::vec3(i, j, maph[i][j]));
			vertices.push_back(glm::vec3(i-1, j, maph[i-1][j]));
			vertices.push_back(glm::vec3(i-1, j-1, maph[i-1][j-1]));
			uvs.push_back(glm::vec2(0, 0));
			uvs.push_back(glm::vec2(1, 0));
			uvs.push_back(glm::vec2(1, 1));
			normals.push_back(glm::vec3(0, 0, 1));
			normals.push_back(glm::vec3(0, 0, 1));
			normals.push_back(glm::vec3(0, 0, 1));

			vertices.push_back(glm::vec3(i, j, maph[i][j]));
			vertices.push_back(glm::vec3(i, j-1, maph[i][j-1]));
			vertices.push_back(glm::vec3(i-1, j-1, maph[i-1][j-1]));
			uvs.push_back(glm::vec2(0, 0));
			uvs.push_back(glm::vec2(0, 1));
			uvs.push_back(glm::vec2(1, 1));
			normals.push_back(glm::vec3(0, 0, 1));
			normals.push_back(glm::vec3(0, 0, 1));
			normals.push_back(glm::vec3(0, 0, 1));
		}

	cout << vertices.size() << ' ' << uvs.size() << ' ' << normals.size() << endl;
}


/*
int main(int argc, char * argv[]) {

    if (argc != 3) {
        cerr << "Usage: ./mapParser <input> <output>" << endl;
        return EXIT_FAILURE;
    }

    ifstream file(argv[1]);
    if (!file.is_open()) {
        cout << '"' << argv[1] << "\" not found." << endl;
        return EXIT_FAILURE;
    }

    ofstream objfile(argv[2]);
    objfile << "# map parsed from " << argv[1] << endl;
}*/
