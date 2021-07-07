#ifndef MESH_H_
#define MESH_H_

#include "Triangle.h"
#include <vector>

class Mesh : public Object
{
public:
	std::vector<Object*> triangles;
	Object* bvh;

	Mesh() {}
	Mesh(const Scene* scene_, int materialId_, Texture* texture_, Texture* normalTexture_, std::vector<Object*> triangles_,
		const Matrix4f& matrix_, bool transform_, const Vec3f& motionVector_, bool motion_);
	bool intersection(const Ray& ray, Hit& hit);
	~Mesh();
};

#endif