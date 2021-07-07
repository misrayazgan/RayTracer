#ifndef RAY_H_
#define RAY_H_

#include "Vec3f.h"

class Object;

class Ray
{
public:
	Vec3f origin;
	Vec3f direction;
	bool isShadowRay;

	Ray() {}
	Ray(const Vec3f& origin_, const Vec3f& direction_) : origin(origin_), direction(direction_), isShadowRay(false) {}
	Ray(const Vec3f& origin_, const Vec3f& direction_, bool shadow_) : origin(origin_), direction(direction_), isShadowRay(shadow_) {}
	Vec3f pointAtParam(float t) const
	{
		return origin + t * direction;
	}
};

struct Hit
{
	bool hitHappened;
	int materialId;
	Vec3f intersectionPoint;
	Vec3f normal;
	float t;
	const Object *obj;
};

#endif