#ifndef RAY_H_
#define RAY_H_

#include "Vec3f.h"
#include "Matrix4f.h"

class Ray
{
public:
	Vec3f origin;
	Vec3f direction;
	bool isInsideObject;
	float time;
	bool indirect;

	Ray() : isInsideObject(false), time(0.0f), indirect(false) {}
	Ray(const Vec3f& origin_, const Vec3f& direction_) : origin(origin_), direction(direction_), isInsideObject(false), time(0.0f) {}
	Ray(const Vec3f& origin_, const Vec3f& direction_, float time_) : origin(origin_), direction(direction_), isInsideObject(false), time(time_) {}
	Ray(const Vec3f& origin_, const Vec3f& direction_, bool inside_, float time_)
		: origin(origin_), direction(direction_), isInsideObject(inside_), time(time_) {}
	Vec3f pointAtParam(float t) const
	{
		return origin + t * direction;
	}
};

#endif