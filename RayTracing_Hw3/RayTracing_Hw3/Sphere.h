#ifndef SPHERE_H_
#define SPHERE_H_

#include "Vec3f.h"
#include "Object.h"

class Sphere : public Object
{
public:
	const int centerVertexId;
	float radius;

	Sphere(const Scene* scene_, const int center_, float radius_, int material_, 
		const Matrix4f& matrix_, bool transform_, const Vec3f& motionVector_, bool motion_);
	bool intersection(const Ray& ray, Hit& hit);
};

#endif