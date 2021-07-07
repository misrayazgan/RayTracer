#include "Light.h"

// Each thread works on separate wi and pointOnLight objects.
thread_local Vec3f AreaLight::wi;
thread_local Vec3f AreaLight::pointOnLight;

AreaLight::AreaLight(const Vec3f& position_, const Vec3f& normal_, float extent_, const Vec3f& radiance_)
	: position(position_), normal(normal_), extent(extent_), radiance(radiance_)
{
	// Construct orthonormal basis uvn.
	Vec3f nPrime = normal;
	int minIdx = nPrime.getAbsMinElementIndex();
	nPrime[minIdx] = 1.0f;

	u = nPrime.crossProduct(normal).unitVector();
	v = u.crossProduct(normal).unitVector();

	// Initialize uniform random distribution to sample points on the surface of the area light later.
	randGenerator.seed(std::chrono::system_clock::now().time_since_epoch().count());
	distribution = std::uniform_real_distribution<float>(-0.5f, 0.5f);
}

Vec3f AreaLight::calculateWi(const Vec3f& intersectionPoint, const Vec3f& normal)
{
	// Sample a uniform random point on area light.
	float e1 = distribution(randGenerator);
	float e2 = distribution(randGenerator);
	pointOnLight = position + extent * (e1 * u + e2 * v);

	wi = (pointOnLight - intersectionPoint).unitVector();
	return wi;
}

float AreaLight::calculateDistance(const Vec3f& intersectionPoint)
{
	float dist = (pointOnLight - intersectionPoint).length();
	return dist;
}

Vec3f AreaLight::calculateIrradiance(const Vec3f& intersectionPoint)
{
	float area = extent * extent;
	float lightDistance = calculateDistance(intersectionPoint);

	// wi is already computed when calculateWi was called.
	Vec3f l = -1 * wi;
	float cosThetaArea = abs(l.dotProduct(normal));
	float dwi = (area * cosThetaArea) / (lightDistance * lightDistance);
	Vec3f irradiance = radiance * dwi;

	return irradiance;
}