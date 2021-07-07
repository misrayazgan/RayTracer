#include "Light.h"

Vec3f SpotLight::calculateWi(const Vec3f& intersectionPoint, const Vec3f& normal)
{
	Vec3f wi = (position - intersectionPoint).unitVector();
	return wi;
}

float SpotLight::calculateDistance(const Vec3f& intersectionPoint)
{
	float dist = (position - intersectionPoint).length();
	return dist;
}

Vec3f SpotLight::calculateIrradiance(const Vec3f& intersectionPoint)
{
	float falloff = calculateFalloff(intersectionPoint);

	float lightDistance = calculateDistance(intersectionPoint);
	Vec3f irradiance = falloff * (intensity / (lightDistance * lightDistance));

	return irradiance;
}

float SpotLight::calculateFalloff(const Vec3f& intersectionPoint)
{
	// Find angle between direction and -wi
	Vec3f wi = calculateWi(intersectionPoint, Vec3f());
	float cosTheta = direction.unitVector().dotProduct(-1 * wi);
	float theta = acos(cosTheta);

	if (theta < (beta / 2.0f))
	{
		// No falloff
		return 1.0f;
	}

	if (theta > (alpha / 2.0f))
	{
		// No light
		return 0.0f;
	}

	float cosAlphaHalf = cos(alpha / 2.0f);
	float cosBetaHalf = cos(beta / 2.0f);

	float falloff = (cosTheta - cosAlphaHalf) / (cosBetaHalf - cosAlphaHalf);
	falloff = pow(falloff, 4);

	return falloff;
}