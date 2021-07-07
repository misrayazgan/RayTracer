#include "Sphere.h"
#include "Scene.h"

Sphere::Sphere(const Scene* scene_, const int center_, float radius_, int material_, 
	const Matrix4f& matrix_, bool transform_, const Vec3f& motionVector_, bool motion_)
	: Object(scene_, material_, matrix_, transform_, motionVector_, motion_), centerVertexId(center_), radius(radius_)
{
	Vec3f center = scene->vertexData[centerVertexId].position;
	Vec3f r = Vec3f(radius, radius, radius);
	Vec3f minCorner = center - r;
	Vec3f maxCorner = center + r;

	boundingBox = BoundingBox(minCorner, maxCorner);
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

bool Sphere::intersection(const Ray& ray, Hit& hit)
{
	bool result = false;

	Ray transformedRay = transformRay(ray);
	Vec3f center = scene->vertexData[centerVertexId].position;

	float a = transformedRay.direction.dotProduct(transformedRay.direction);
	float b = 2 * transformedRay.direction.dotProduct(transformedRay.origin - center);
	float c = (transformedRay.origin - center).dotProduct(transformedRay.origin - center) - radius * radius;

	float discriminant = b*b - 4*a*c;

	if (discriminant < 0.0f)
	{
		// No intersection
		return result;
	}
	else
	{
		// Intersection at 1 or 2 points
		float t1 = (-1 * b + sqrt(discriminant)) / (2 * a);
		float t2 = (-1 * b - sqrt(discriminant)) / (2 * a);
		float t = std::min(t1, t2);

		// If ray's origin is on the surface of the sphere, t1 = 0.
		// If ray's origin is inside the sphere, t2 < 0.
		if (t1 < 0.0f)
		{
			t = t2;
		}
		else if (t2 < 0.0f)
		{
			t = t1;
		}

		Vec3f intersectionPoint = transformedRay.pointAtParam(t);
		Vec3f surfaceNormal = (intersectionPoint - center).unitVector();

		hit.t = t;
		hit.materialId = materialId;
		hit.intersectionPoint = intersectionPoint;
		hit.normal = surfaceNormal;

		hit = transformHit(hit, transformedRay.time);

		result = true;
	}

	return result;
}