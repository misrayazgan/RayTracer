#ifndef BOUNDINGBOX_H_
#define BOUNDINGBOX_H_

#include "Ray.h"

class BoundingBox
{
public:
	Vec3f minCorner;	// (xmin, ymin, zmin)
	Vec3f maxCorner;	// (xmax, ymax, zmax)
	Vec3f diagonal;
	Vec3f center;

	BoundingBox() : minCorner(kInf, kInf, kInf), maxCorner(-kInf, -kInf, -kInf) {}
	BoundingBox(Vec3f minCorner_, Vec3f maxCorner_);
	float intersection(const Ray& ray);
	void mergeBoundingBox(const BoundingBox& boundingBox);
};

#endif