#ifndef MESH_H_
#define MESH_H_

#include "Triangle.h"
#include <vector>

struct Mesh
{
	int materialId;
	std::vector<Triangle> triangles;
};

#endif