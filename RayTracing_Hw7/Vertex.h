#ifndef VERTEX_H_
#define VERTEX_H_

#include "Vec3f.h"

class Vertex
{
public:
	Vertex() {}
	Vertex(const Vec3f& position_) : position(position_.x, position_.y, position_.z), vertexNormal(0.0f, 0.0f, 0.0f), numberOfAdjacentTriangles(0) {}
	void addToVertexNormal(const Vec3f& normal)
	{
		vertexNormal += normal;
		numberOfAdjacentTriangles++;
	}

	Vec3f position;
	Vec3f vertexNormal;
	int numberOfAdjacentTriangles;
};

#endif