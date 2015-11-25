#include "CParticle.h"

inline float rand01() {
	return float(rand()) / RAND_MAX;
}

inline float randWithin(vec2 & range) {
	return range[0] + float(rand01())*(range[1] - range[0]);
}

void CParticle::step() {

	if (pos.size() < max_particle) 
		if (rand01() < create_probability) {
			vec3 nextpos(
				randWithin(posx),
				randWithin(posy),
				randWithin(posz)
				);
			vec3 nextvel = vel + vec3(rand01()*vnoise.x, rand01()*vnoise.y, rand01()*vnoise.z);
//			printf("%.3f %.3f %.3f\n", nextpos.x, nextpos.y, nextpos.z);
			pos.push_back(nextpos);
			velocity.push_back(nextvel);
		}

	for (int i = 0; i < pos.size(); ++i) {
		pos[i] += velocity[i];

		// special judge
		if (pos[i].z < 0) {
			// on ground, reset
			pos[i] = vec3(
				randWithin(posx),
				randWithin(posy),
				randWithin(posz)
			);
			velocity[i] = vel + vec3(rand01()*vnoise.x, rand01()*vnoise.y, rand01()*vnoise.z);
		}
	}

	render();
}

CParticle::CParticle(
	int max_particle,
	float create_probability,
	vec2 posx, vec2 posy, vec2 posz,
	vec3 vel, vec3 vnoise
	) {

	this->max_particle = max_particle;
	this->create_probability = create_probability;
	this->posx = posx;
	this->posy = posy;
	this->posz = posz;
	this->vel = vel;
	this->vnoise = vnoise;

	__getTetrahedron(0.5);
}

#define add(a, b, c, n)\
	indexed_vertices.push_back(__p[a]+pos[i]);\
	indexed_vertices.push_back(__p[b]+pos[i]);\
	indexed_vertices.push_back(__p[c]+pos[i]);\
	indexed_uvs.push_back(vec2(0,0));\
	indexed_uvs.push_back(vec2(1,0));\
	indexed_uvs.push_back(vec2(0,1));\
	indexed_normals.push_back(n);\
	indexed_normals.push_back(n);\
	indexed_normals.push_back(n);\
	indices.push_back(indexed_vertices.size()-3);\
	indices.push_back(indexed_vertices.size()-2);\
	indices.push_back(indexed_vertices.size()-1);

void CParticle::render() {
	indexed_normals.clear();
	indexed_uvs.clear();
	indexed_vertices.clear();
	indices.clear();
	this->releaseBuffer();

	for (int i = 0; i < pos.size(); ++i) {
		add(0, 1, 2, vec3(1, 1, 1));
		add(0, 1, 3, vec3(-1, 0, 0));
		add(0, 2, 3, vec3(0, -1, 0));
		add(1, 2, 3, vec3(0, 0, -1));
	}

	if (indexed_vertices.size()) {
//  		this->getIndex();
		this->getBuffer();
	}
}

void CParticle::__getTetrahedron(float __r) {
	__p[0] = vec3(0, 0, __r);
	__p[1] = vec3(0, __r, 0);
	__p[2] = vec3(__r, 0, 0);
	__p[3] = vec3(0, 0, 0);
}

CParticle::CParticle() {
	__getTetrahedron(0.1);
}

CParticle::~CParticle() {

}