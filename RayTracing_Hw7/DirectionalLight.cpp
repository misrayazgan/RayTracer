#include "Light.h"

Vec3f DirectionalLight::calculateWi(const Vec3f& intersectionPoint, const Vec3f& normal)
{
	Vec3f wi = (-1 * direction).unitVector();
	return wi;
}

float DirectionalLight::calculateDistance(const Vec3f& intersectionPoint)
{
	float dist = std::numeric_limits<float>::max();
	return dist;
}

Vec3f DirectionalLight::calculateIrradiance(const Vec3f& intersectionPoint)
{
	return radiance;
}