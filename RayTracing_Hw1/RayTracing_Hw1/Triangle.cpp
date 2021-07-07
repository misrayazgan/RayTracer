#include "Triangle.h"
#include "Scene.h"

const float epsilon = 0.000001;

Triangle::Triangle(const Scene* scene_, int v0_, int v1_, int v2_, int material_)
	: Object(scene_, material_), v0(v0_), v1(v1_), v2(v2_)
{
	// Calculate surface normal
	Vec3f a = scene->vertexData[v0];
	Vec3f b = scene->vertexData[v1];
	Vec3f c = scene->vertexData[v2];

	normal = (b - a).crossProduct(c - a).unitVector();
}

float Triangle::determinant(const Vec3f &v0, const Vec3f &v1, const Vec3f &v2)
{
	return v0.x * (v1.y * v2.z - v2.y * v1.z)
		 + v0.y * (v2.x * v1.z - v1.x * v2.z)
		 + v0.z * (v1.x * v2.y - v1.y * v2.x);
}

Hit Triangle::intersection(const Ray& ray)
{
	Hit hit;
	hit.hitHappened = false;

	Vec3f o = ray.origin;
	Vec3f d = ray.direction;
	Vec3f a = scene->vertexData[v0];
	Vec3f b = scene->vertexData[v1];
	Vec3f c = scene->vertexData[v2];

	float detA = determinant(a - b, a - c, d);
	if (detA == 0.0f)
	{
		return hit;
	}

	float t = (determinant(a - b, a - c, a - o)) / detA;
	if (t < 0.0f - epsilon)
	{
		return hit;
	}

	float gamma = (determinant(a - b, a - o, d)) / detA;
	if (gamma < 0.0f - epsilon || gamma > 1.0f + epsilon)
	{
		return hit;
	}

	float beta = (determinant(a - o, a - c, d)) / detA;
	if (beta < 0.0f - epsilon || beta > (1.0f + epsilon - gamma))
	{
		return hit;
	}

	hit.hitHappened = true;
	hit.t = t;
	hit.materialId = materialId;
	hit.intersectionPoint = ray.pointAtParam(t);
	hit.normal = normal;
	hit.obj = this;

	return hit;
}