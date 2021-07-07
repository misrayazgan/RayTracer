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

std::vector<Vec3f> BoundingBox::getVertices() const
{
	std::vector<Vec3f> vertices;
	vertices.push_back(minCorner);
	vertices.push_back(Vec3f(minCorner.x, minCorner.y, maxCorner.z));
	vertices.push_back(Vec3f(minCorner.x, maxCorner.y, minCorner.z));
	vertices.push_back(Vec3f(minCorner.x, maxCorner.y, maxCorner.z));
	vertices.push_back(Vec3f(maxCorner.x, minCorner.y, minCorner.z));
	vertices.push_back(Vec3f(maxCorner.x, minCorner.y, maxCorner.z));
	vertices.push_back(Vec3f(maxCorner.x, maxCorner.y, minCorner.z));
	vertices.push_back(maxCorner);

	return vertices;
}

void BoundingBox::applyTransformation(const Matrix4f& transformationMat)
{
	std::vector<Vec3f> vertices = getVertices();

	// Apply transformation to all vertices.
	for (int i = 0; i < vertices.size(); i++)
	{
		vertices[i] = transformationMat.multiplyWithPoint(vertices[i]);
	}

	// Vertex coordinates are changed, find new min and max corners.
	minCorner = vertices[0];
	maxCorner = vertices[0];
	for (int i = 1; i < vertices.size(); i++)
	{
		minCorner.x = std::min(minCorner.x, vertices[i].x);
		minCorner.y = std::min(minCorner.y, vertices[i].y);
		minCorner.z = std::min(minCorner.z, vertices[i].z);

		maxCorner.x = std::max(maxCorner.x, vertices[i].x);
		maxCorner.y = std::max(maxCorner.y, vertices[i].y);
		maxCorner.z = std::max(maxCorner.z, vertices[i].z);
	}
}