#include "BoundingBox.h"

BoundingBox::BoundingBox(Vec3f minCorner_, Vec3f maxCorner_)
	: minCorner(minCorner_), maxCorner(maxCorner_)
{
	diagonal = maxCorner - minCorner;
	center = (minCorner + maxCorner) / 2;
}

float BoundingBox::intersection(const Ray& ray)
{
	float tNearMax = -1 * kInf;	// t1
	float tFarMin = kInf;		// t2

	// For x, y, z planes
	for (int i = 0; i < 3; i++)
	{
		float a = 1 / ray.direction[i];

		float tmin = a * (minCorner[i] - ray.origin[i]);
		float tmax = a * (maxCorner[i] - ray.origin[i]);

		if (a < 0.0f)
		{
			std::swap(tmin, tmax);
		}

		// Find max entrance t and min exit t.
		if (tmin > tNearMax)
		{
			tNearMax = tmin;
		}
		if (tmax < tFarMin)
		{
			tFarMin = tmax;
		}

		if (tNearMax > tFarMin)
		{
			return kInf;
		}
	}

	if (tNearMax < 0.0f)
	{
		std::swap(tNearMax, tFarMin);
	}

	return tNearMax;
}

void BoundingBox::mergeBoundingBox(const BoundingBox& boundingBox)
{
	minCorner.x = std::min(minCorner.x, boundingBox.minCorner.x);
	minCorner.y = std::min(minCorner.y, boundingBox.minCorner.y);
	minCorner.z = std::min(minCorner.z, boundingBox.minCorner.z);

	maxCorner.x = std::max(maxCorner.x, boundingBox.maxCorner.x);
	maxCorner.y = std::max(maxCorner.y, boundingBox.maxCorner.y);
	maxCorner.z = std::max(maxCorner.z, boundingBox.maxCorner.z);

	diagonal = maxCorner - minCorner;
	center = (minCorner + maxCorner) / 2;
}