#include "Sphere.h"
#include "Scene.h"

Hit Sphere::intersection(const Ray& ray)
{
	Hit hit;
	Vec3f center = scene->vertexData[centerVertexId];

	float a = ray.direction.dotProduct(ray.direction);
	float b = 2 * ray.direction.dotProduct(ray.origin - center);
	float c = (ray.origin - center).dotProduct(ray.origin - center) - radius * radius;

	float discriminant = b*b - 4*a*c;

	if (discriminant < 0.0f)
	{
		// No intersection
		hit.hitHappened = false;
	}
	else
	{
		// Intersection at 1 or 2 points
		float t1 = (-1 * b + sqrt(discriminant)) / (2 * a);
		float t2 = (-1 * b - sqrt(discriminant)) / (2 * a);
		float t = std::min(t1, t2);

		Vec3f intersectionPoint = ray.pointAtParam(t);
		Vec3f surfaceNormal = (intersectionPoint - center).unitVector();

		hit.hitHappened = true;
		hit.t = t;
		hit.obj = this;
		hit.materialId = materialId;
		hit.intersectionPoint = intersectionPoint;
		hit.normal = surfaceNormal;
	}

	return hit;
}