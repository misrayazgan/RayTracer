#include "Light.h"

Vec3f PointLight::calculateWi(const Vec3f& intersectionPoint, const Vec3f& normal)
{
	Vec3f wi = (position - intersectionPoint).unitVector();
	return wi;
}

float PointLight::calculateDistance(const Vec3f& intersectionPoint)
{
	float dist = (position - intersectionPoint).length();
	return dist;
}

Vec3f PointLight::calculateIrradiance(const Vec3f& intersectionPoint)
{
	float lightDistance = calculateDistance(intersectionPoint);
	Vec3f irradiance = intensity / (lightDistance * lightDistance);

	return irradiance;
}