#include "CSkybox.h"
#include <glm\glm.hpp>
using glm::vec3;
using glm::vec2;

static void AddSameNormalRect(CObj &obj, vec3 p1, vec3 p2, vec3 p3, vec3 p4, vec2 uv1, vec2 uv2, vec2 uv3, vec2 uv4, vec3 n) {
	obj.vertices.push_back(p1);
	obj.vertices.push_back(p2);
	obj.vertices.push_back(p3);
	obj.uvs.push_back(uv1);
	obj.uvs.push_back(uv2);
	obj.uvs.push_back(uv3);
	obj.normals.push_back(n);
	obj.normals.push_back(n);
	obj.normals.push_back(n);
	obj.vertices.push_back(p1);
	obj.vertices.push_back(p3);
	obj.vertices.push_back(p4);
	obj.uvs.push_back(uv1);
	obj.uvs.push_back(uv3);
	obj.uvs.push_back(uv4);
	obj.normals.push_back(n);
	obj.normals.push_back(n);
	obj.normals.push_back(n);
}

CSkybox::CSkybox(float x1, float y1, float x2, float y2, float height) {
	this->x1 = x1;
	this->y1 = y1;
	this->x2 = x2;
	this->y2 = y2;
	this->height = height;

	vec3 p1(x1, y1, 0);
	vec3 p2(x2, y1, 0);
	vec3 p3(x2, y2, 0);
	vec3 p4(x1, y2, 0);
	vec3 p8(x1, y1, height);
	vec3 p7(x2, y1, height);
	vec3 p6(x2, y2, height);
	vec3 p5(x1, y2, height);

	AddSameNormalRect(obj, p1, p2, p3, p4, vec2(0.25, 0.333333334), vec2(0.50, 0.333333334), vec2(0.50, 0.00), vec2(0.25, 0.00), vec3(0, 0, 1));
	AddSameNormalRect(obj, p5, p6, p7, p8, vec2(0.25, 1.00), vec2(0.50, 1.00), vec2(0.50, 0.6666666666), vec2(0.25, 0.6666666666), vec3(0, 0, -1));
	AddSameNormalRect(obj, p1, p2, p7, p8, vec2(0.25, 0.333333334), vec2(0.50, 0.333333334), vec2(0.50, 0.6666666666), vec2(0.25, 0.6666666666), vec3(0, 1, 0));
	AddSameNormalRect(obj, p5, p4, p3, p6, vec2(1.00, 0.6666666666), vec2(1.00, 0.333333334), vec2(0.75, 0.333333334), vec2(0.75, 0.6666666666), vec3(0, -1, 0));
	AddSameNormalRect(obj, p7, p2, p3, p6, vec2(0.50, 0.6666666666), vec2(0.50, 0.333333334), vec2(0.75, 0.333333334), vec2(0.75, 0.6666666666), vec3(-1, 0, 0));
	AddSameNormalRect(obj, p5, p4, p1, p8, vec2(0.00, 0.6666666666), vec2(0.00, 0.333333334), vec2(0.25, 0.333333334), vec2(0.25, 0.6666666666), vec3(1, 0, 0));

	obj.getIndex();
	obj.getBuffer();
}


CSkybox::CSkybox() {
}

CSkybox::~CSkybox() {
}

