#ifndef TRIANGLE_H_
#define TRIANGLE_H_

#include "Vec3f.h"
#include "Object.h"

enum ShadingMode
{
	SHADINGMODE_FLAT = 0,
	SHADINGMODE_SMOOTH
};

class Triangle : public Object
{
public:
	int v0, v1, v2;
	Vec3f normal;
	float area;
	ShadingMode shadingMode;

	Triangle(const Scene* scene_, int v0_, int v1_, int v2_, int material_, ShadingMode shadingMode_);
	bool intersection(const Ray& ray, Hit& hit);

private:
	float determinant(const Vec3f &v0, const Vec3f &v1, const Vec3f &v2);
};

#endif