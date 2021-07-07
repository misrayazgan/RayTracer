#include "MeshInstance.h"

MeshInstance::MeshInstance(const Scene* scene_, int materialId_, Object* baseMeshBVH_, const Matrix4f& matrix_, bool transform_,
	const Vec3f& motionVector_, bool motion_)
	: Object(scene_, materialId_, matrix_, transform_, motionVector_, motion_), baseMeshBVH(baseMeshBVH_)
{
	// Get base mesh's bvh's bounding box, which has no transformations applied.
	boundingBox = baseMeshBVH->getBoundingBox();
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

bool MeshInstance::intersection(const Ray& ray, Hit& hit)
{
	// Apply transformations wrt this mesh instance.
	Ray transformedRay = transformRay(ray);

	bool result = baseMeshBVH->intersection(transformedRay, hit);
	if (result == true)
	{
		hit = transformHit(hit, transformedRay.time);

		// Intersection is computed using base mesh's bvh, need to override object specific features like material.
		// Use this mesh instance's material instead of baseMesh's triangle's material.
		hit.materialId = materialId;
	}

	return result;
}


MeshInstance::~MeshInstance()
{
	if (baseMeshBVH)
	{
		delete baseMeshBVH;
	}
}