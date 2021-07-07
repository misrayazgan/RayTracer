#include "Triangle.h"
#include "Scene.h"

Triangle::Triangle(const Scene* scene_, int v0_, int v1_, int v2_, int material_, ShadingMode shadingMode_)
	: Object(scene_, material_), v0(v0_), v1(v1_), v2(v2_), shadingMode(shadingMode_)
{
	// Calculate surface normal
	Vec3f a = scene->vertexData[v0].position;
	Vec3f b = scene->vertexData[v1].position;
	Vec3f c = scene->vertexData[v2].position;

	normal = (b - a).crossProduct(c - a).unitVector();

	// Calculate triangle's area
	area = (b - a).crossProduct(c - a).length() / 2;

	// Calculate triangle's bounding box
	float xmin = std::min(std::min(a.x, b.x), c.x);
	float ymin = std::min(std::min(a.y, b.y), c.y);
	float zmin = std::min(std::min(a.z, b.z), c.z);
	Vec3f minCorner = Vec3f(xmin, ymin, zmin);

	float xmax = std::max(std::max(a.x, b.x), c.x);
	float ymax = std::max(std::max(a.y, b.y), c.y);
	float zmax = std::max(std::max(a.z, b.z), c.z);
	Vec3f maxCorner = Vec3f(xmax, ymax, zmax);

	boundingBox = BoundingBox(minCorner, maxCorner);
}

float Triangle::determinant(const Vec3f &v0, const Vec3f &v1, const Vec3f &v2)
{
	return v0.x * (v1.y * v2.z - v2.y * v1.z)
		 + v0.y * (v2.x * v1.z - v1.x * v2.z)
		 + v0.z * (v1.x * v2.y - v1.y * v2.x);
}

bool Triangle::intersection(const Ray& ray, Hit& hit)
{
	bool result = false;

	Vec3f o = ray.origin;
	Vec3f d = ray.direction;
	Vec3f a = scene->vertexData[v0].position;
	Vec3f b = scene->vertexData[v1].position;
	Vec3f c = scene->vertexData[v2].position;

	float detA = determinant(a - b, a - c, d);
	if (detA == 0.0f)
	{
		return result;
	}

	float t = (determinant(a - b, a - c, a - o)) / detA;
	if (t < 0.0f - epsilon)
	{
		return result;
	}

	float gamma = (determinant(a - b, a - o, d)) / detA;
	if (gamma < 0.0f - epsilon || gamma > 1.0f + epsilon)
	{
		return result;
	}

	float beta = (determinant(a - o, a - c, d)) / detA;
	if (beta < 0.0f - epsilon || beta > (1.0f + epsilon - gamma))
	{
		return result;
	}

	hit.t = t;
	hit.materialId = materialId;
	hit.intersectionPoint = ray.pointAtParam(t);
	hit.obj = this;

	if (shadingMode == SHADINGMODE_FLAT)
	{
		hit.normal = normal;
	}
	else if (shadingMode == SHADINGMODE_SMOOTH)
	{
		hit.normal = (1 - beta - gamma) * scene->vertexData[v0].vertexNormal
								 + beta * scene->vertexData[v1].vertexNormal
								 + gamma * scene->vertexData[v2].vertexNormal;
	}

	result = true;
	return result;
}