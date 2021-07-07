#include "Mesh.h"
#include "BVH.h"

Mesh::Mesh(const Scene* scene_, int materialId_, std::vector<Object*> triangles_, const Matrix4f& matrix_, bool transform_, 
	const Vec3f& motionVector_, bool motion_)
	: Object(scene_, materialId_, matrix_, transform_, motionVector_, motion_), triangles(triangles_)
{
	bvh = new BVH(triangles, 0, triangles.size(), 0);

	// This sets only this mesh's bounding box, it does not affect the bvh's bounding box,
	// no transformations are applied to bvh's bounding box.
	boundingBox = bvh->getBoundingBox();
	if (transform == true)
	{
		// Transform bounding box to world coords.
		boundingBox.applyTransformation(transformationMatrix);
	}

	if (motionBlur == true)
	{
		Vec3f minCornerLowerBound = boundingBox.minCorner - motionVector;
		Vec3f maxCornerUpperBound = boundingBox.maxCorner + motionVector;

		boundingBox.mergeBoundingBox(BoundingBox(minCornerLowerBound, maxCornerUpperBound));
	}
}

bool Mesh::intersection(const Ray& ray, Hit& hit)
{
	Ray transformedRay = transformRay(ray);

	bool result = bvh->intersection(transformedRay, hit);
	if (result == true)
	{
		hit = transformHit(hit, transformedRay.time);
	}

	return result;
}

Mesh::~Mesh()
{
	if (bvh)
	{
		delete bvh;
	}
}