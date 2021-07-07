#ifndef RAY_H_
#define RAY_H_

#include "Vec3f.h"
#include "Matrix4f.h"

class Object;

class Ray
{
public:
	Vec3f origin;
	Vec3f direction;
	bool isInsideObject;
	float time;

	Ray() : isInsideObject(false), time(0.0f) {}
	Ray(const Vec3f& origin_, const Vec3f& direction_) : origin(origin_), direction(direction_), isInsideObject(false), time(0.0f) {}
	Ray(const Vec3f& origin_, const Vec3f& direction_, float time_) : origin(origin_), direction(direction_), isInsideObject(false), time(time_) {}
	Ray(const Vec3f& origin_, const Vec3f& direction_, bool inside_, float time_)
		: origin(origin_), direction(direction_), isInsideObject(inside_), time(time_) {}
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
};

#endif