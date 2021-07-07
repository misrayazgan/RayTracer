#ifndef MESH_H_
#define MESH_H_

#include "Triangle.h"
#include <vector>

class Mesh : public Object
{
public:
	std::vector<Triangle> triangles;

	Mesh() {}
	Mesh(const Scene* scene_, int materialId_, std::vector<Triangle> triangles_)
		: Object(scene_, materialId_), triangles(triangles_) {}
	bool intersection(const Ray& ray, Hit& hit)
	{
		// TO DO: will be overridden
		return false;
	}
};

#endif