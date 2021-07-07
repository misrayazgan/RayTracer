#include "BVH.h"

BVH::BVH(std::vector<Object*>& objects, int start, int end, int axis)
{
	int numberOfObjects = end - start;

	if (numberOfObjects == 0)
	{
		// No objects in the scene
		left = NULL;
		right = NULL;
		boundingBox = BoundingBox(Vec3f(), Vec3f());
	}
	else if (numberOfObjects == 1)
	{
		left = objects[start];
		right = NULL;
		boundingBox = objects[start]->getBoundingBox();
	}
	else if (numberOfObjects == 2)
	{
		left = objects[start];
		right = objects[end - 1];
		boundingBox = objects[start]->getBoundingBox();
		boundingBox.mergeBoundingBox(objects[end - 1]->getBoundingBox());
	}
	else
	{
		for (int i = start; i < end; i++)
		{
			boundingBox.mergeBoundingBox(objects[i]->getBoundingBox());
		}

		int mid = start;
		float center = boundingBox.center[axis];
		for (int i = start; i < end; i++)
		{
			float objectCenter = objects[i]->getBoundingBox().center[axis];
			if (objectCenter < center)
			{
				std::swap(objects[i], objects[mid]);
				mid++;
			}
		}

		// If space partitioning fails, partition by taking the middle object.
		if (mid == start || mid == end)
		{
			mid = start + (end - start) / 2;
		}

		left = new BVH(objects, start, mid, (axis + 1) % 3);
		right = new BVH(objects, mid, end, (axis + 1) % 3);
	}
}

bool BVH::intersection(const Ray& ray, Hit& hit)
{
	bool result = false;
	float t = boundingBox.intersection(ray);
	if (t < 0.0f || t == kInf)
	{
		// No intersection with this bounding box
		return result;
	}

	if (left)
	{
		Hit hitLeft = Hit();
		bool leftResult = left->intersection(ray, hitLeft);
		if (leftResult == true && hitLeft.t < hit.t && hitLeft.t > 0.0f)
		{
			hit = hitLeft;
			result = true;
		}
	}

	if (right)
	{
		Hit hitRight = Hit();
		bool rightResult = right->intersection(ray, hitRight);
		if (rightResult == true && hitRight.t < hit.t && hitRight.t > 0.0f)
		{
			hit = hitRight;
			result = true;
		}
	}

	return result;
}

BVH::~BVH()
{
	if (left)
	{
		delete left;
	}

	if (right)
	{
		delete right;
	}
}