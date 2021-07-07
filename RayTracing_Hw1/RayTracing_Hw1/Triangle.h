#ifndef TRIANGLE_H_
#define TRIANGLE_H_

#include "Vec3f.h"
#include "Object.h"

class Triangle : public Object
{
public:
	int v0, v1, v2;
	Vec3f normal;

	Triangle(const Scene* scene_, int v0_, int v1_, int v2_, int material_);
	Hit intersection(const Ray& ray);

private:
	float determinant(const Vec3f &v0, const Vec3f &v1, const Vec3f &v2);
};

#endif