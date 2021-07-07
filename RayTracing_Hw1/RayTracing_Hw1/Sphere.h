#ifndef SPHERE_H_
#define SPHERE_H_

#include "Vec3f.h"
#include "Object.h"

class Sphere : public Object
{
public:
	const int centerVertexId;
	float radius;

	Sphere(const Scene* scene_, const int center_, float radius_, int material_) 
		: Object(scene_, material_), centerVertexId(center_), radius(radius_) {}
	Hit intersection(const Ray& ray);
};

#endif