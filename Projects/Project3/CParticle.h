#pragma once

#ifndef __CPARTICLE__
#define __CPARTICLE__

#include <vector>
#include <glm/glm.hpp>
#include "CObj.h"

using std::vector;
using glm::vec3;
using glm::vec2;

class CParticle : public CObj {
public:
	vector<vec3> pos;
	vector<vec3> velocity;

	CParticle();
	CParticle(
		int max_particle,
		float create_probability,
		vec2 posx, vec2 posy, vec2 posz,
		vec3 vel, vec3 vnoise
	);
	~CParticle();

	void step();
	void render();

	int max_particle;
	float create_probability;
	vec2 posx, posy, posz;
	vec3 vel, vnoise;

	void __getTetrahedron(float __r);
	vec3 __p[4];
};

#endif