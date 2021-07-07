#ifndef AREALIGHT_H_
#define AREALIGHT_H_

#include "Vec3f.h"

class AreaLight
{
public:
	Vec3f position;
	Vec3f normal;
	float extent;
	Vec3f radiance;
	Vec3f u;
	Vec3f v;

	AreaLight(const Vec3f& position_, const Vec3f& normal_, float extent_, const Vec3f& radiance_)
		: position(position_), normal(normal_), extent(extent_), radiance(radiance_)
	{
		// Construct orthonormal basis uvn.
		Vec3f nPrime = normal;
		int minIdx = nPrime.getAbsMinElementIndex();
		nPrime[minIdx] = 1.0f;

		u = nPrime.crossProduct(normal).unitVector();
		v = u.crossProduct(normal).unitVector();
	}
};


#endif
