#ifndef RAY_H_
#define RAY_H_

#include "Vec3f.h"

class Object;

class Ray
{
public:
	Vec3f origin;
	Vec3f direction;
	bool isInsideObject;

	Ray() : isInsideObject(false) {}
	Ray(const Vec3f& origin_, const Vec3f& direction_) : origin(origin_), direction(direction_), isInsideObject(false) {}
	Ray(const Vec3f& origin_, const Vec3f& direction_, bool inside_)
		: origin(origin_), direction(direction_), isInsideObject(inside_) {}
	Vec3f pointAtParam(float t) const
	{
		return origin + t * direction;
	}
};

struct Hit
{
	int materialId;
	Vec3f intersectionPoint;
	Vec3f normal;
	float t;
	const Object *obj;
};

#endif