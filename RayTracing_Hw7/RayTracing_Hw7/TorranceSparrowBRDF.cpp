#include "BRDF.h"

Vec3f TorranceSparrowBRDF::getBRDFValue(const Hit& hit, const Material& material, const Vec3f& wi, const Vec3f& wo, const Vec3f& irradiance) const
{
	Vec3f brdfValue = Vec3f();
	float cosTheta = std::max(0.0f, wi.dotProduct(hit.normal));

	if (cosTheta > 0.0f)
	{
		// theta < 90°
		Vec3f kd = material.diffuse;
		Vec3f ks = material.specular;

		Vec3f wh = (wi + wo).unitVector();
		float cosAlphaH = std::max(0.0f, wh.dotProduct(hit.normal));
		float cosBeta = std::max(0.0f, wo.dotProduct(wh));
		float cosPhi = std::max(0.0f, wo.dotProduct(hit.normal));

		float D = blinnDistribution(cosAlphaH);
		float G = geometryTerm(wi, wo, wh, hit.normal);
		float F = fresnelReflectance(cosBeta, material);

		if (kdfresnel == true)
		{
			kd = (1.0f - F) * kd;
		}

		brdfValue = (kd / PI) + (ks * ((D * F * G) / (4 * cosTheta * (cosPhi + 0.00001f))));
	}
	// Else: brdf value is (0,0,0).

	return irradiance * brdfValue * cosTheta;
}

float TorranceSparrowBRDF::blinnDistribution(float cosAlpha) const
{
	float result = ((exponent + 2.0f) / (2.0f * PI)) * pow(cosAlpha, exponent);
	return result;
}

float TorranceSparrowBRDF::geometryTerm(const Vec3f& wi, const Vec3f& wo, const Vec3f& wh, const Vec3f& n) const
{
	float left = (2.0f * n.dotProduct(wh) * n.dotProduct(wo)) / (wo.dotProduct(wh));
	float right = (2.0f * n.dotProduct(wh) * n.dotProduct(wi)) / (wo.dotProduct(wh));
	float result = std::min(1.0f, std::min(left, right));

	return result;
}

float TorranceSparrowBRDF::fresnelReflectance(float cosBeta, const Material& material) const
{
	float n = material.refractionIndex;
	float k = material.absorptionIndex;

	// Fresnel equations for conductors
	float rs = ((n*n + k * k) - 2 * n*cosBeta + cosBeta * cosBeta) /
		((n*n + k * k) + 2 * n*cosBeta + cosBeta * cosBeta);
	float rp = ((n*n + k * k)*cosBeta*cosBeta - 2 * n*cosBeta + 1) /
		((n*n + k * k)*cosBeta*cosBeta + 2 * n*cosBeta + 1);

	float fr = (rs + rp) / 2;
	return fr;
}