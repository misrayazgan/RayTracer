#include "Object.h"
#include "Scene.h"

Object::Object(const Scene* scene_, int id_, const Matrix4f& matrix_, bool transform_, const Vec3f& motionVector_, bool motion_)
	: scene(scene_), materialId(id_), transformationMatrix(matrix_), transform(transform_), motionVector(motionVector_), motionBlur(motion_)
{
	// Compute inverse transformation matrix and normal transformation matrix
	inverseTransformationMatrix = transformationMatrix.inverse();
	normalTransformationMatrix = inverseTransformationMatrix.transpose();
}

const BoundingBox& Object::getBoundingBox() const
{
	return boundingBox;
}

Ray Object::transformRay(const Ray& ray) const
{
	// Transform ray to local coordinates.
	Ray transformedRay;

	if (transform == false && motionBlur == false)
	{
		return ray;
	}
	else if (transform == true && motionBlur == false)
	{
		transformedRay.origin = inverseTransformationMatrix.multiplyWithPoint(ray.origin);
		transformedRay.direction = inverseTransformationMatrix.multiplyWithVector(ray.direction);
	}
	else if (motionBlur == true)
	{
		Vec3f motionBlurDistance = motionVector * ray.time;
		Translation translation = Translation(motionBlurDistance);
		Matrix4f invFinalMatrix = translation.getTransformationMatrix().inverse();

		if (transform == true)
		{
			invFinalMatrix = inverseTransformationMatrix * invFinalMatrix;
		}
		
		transformedRay.origin = invFinalMatrix.multiplyWithPoint(ray.origin);
		transformedRay.direction = invFinalMatrix.multiplyWithVector(ray.direction);
	}

	transformedRay.isInsideObject = ray.isInsideObject;
	transformedRay.time = ray.time;

	return transformedRay;
}

Hit Object::transformHit(const Hit& hit, float time) const
{
	// Transform hit to world coordinates.
	Hit transformedHit;

	if (transform == false && motionBlur == false)
	{
		return hit;
	}
	else if (transform == true && motionBlur == false)
	{
		transformedHit.intersectionPoint = transformationMatrix.multiplyWithPoint(hit.intersectionPoint);
		transformedHit.normal = (normalTransformationMatrix.multiplyWithVector(hit.normal)).unitVector();
	}
	else if (transform == false && motionBlur == true)
	{
		Vec3f motionBlurDistance = motionVector * time;
		Translation translation = Translation(motionBlurDistance);
		Matrix4f motionMatrix = translation.getTransformationMatrix();
		Matrix4f invMotionMatrix = motionMatrix.inverse();
		Matrix4f normalMotionMatrix = invMotionMatrix.transpose();

		transformedHit.intersectionPoint = motionMatrix.multiplyWithPoint(hit.intersectionPoint);
		transformedHit.normal = (normalMotionMatrix.multiplyWithVector(hit.normal)).unitVector();
	}
	else
	{
		Vec3f motionBlurDistance = motionVector * time;
		Translation translation = Translation(motionBlurDistance);
		Matrix4f motionMatrix = translation.getTransformationMatrix();
		Matrix4f invMotionMatrix = motionMatrix.inverse();

		Matrix4f finalMatrix = motionMatrix * transformationMatrix;
		Matrix4f finalNormalMatrix = (inverseTransformationMatrix * invMotionMatrix).transpose();

		transformedHit.intersectionPoint = finalMatrix.multiplyWithPoint(hit.intersectionPoint);
		transformedHit.normal = (finalNormalMatrix.multiplyWithVector(hit.normal)).unitVector();
	}

	transformedHit.materialId = hit.materialId;
	transformedHit.t = hit.t;

	return transformedHit;
}