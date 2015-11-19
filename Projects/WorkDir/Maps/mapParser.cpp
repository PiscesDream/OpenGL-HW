// dropped at 2015/11/19 11:38:21
// moved to project
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include "mapParser.hpp"
#include <glm/glm.hpp>

using namespace std;

#define __DEBUG__

inline int parseChar(char c) {
    for (register int i = 0; i < MAPTABLELEN; ++i) 
        if (c==MAPTABLE[i]) return i;
    return -1;
}


void parseMap(ifstream file) {
    // start to read file
    // exception handler should be added
    int n, m, sx, sy;
    float w, l, h; 
    int map[MAXN+2][MAXN+2] = {0};

    file >> n >> m >> w >> l >> h; 
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
}


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
}
