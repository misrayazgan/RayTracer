#include "BRDF.h"

Vec3f PhongBRDF::getBRDFValue(const Hit& hit, const Material& material, const Vec3f& wi, const Vec3f& wo, const Vec3f& irradiance) const
{
	Vec3f brdfValue = Vec3f();
	float cosTheta = std::max(0.0f, wi.dotProduct(hit.normal));

	if (cosTheta > 0.0f)
	{
		// theta < 90°
		Vec3f kd = material.diffuse;
		Vec3f ks = material.specular;

		Vec3f wr = (2 * (hit.normal * (wi.dotProduct(hit.normal))) - wi).unitVector();
		float cosAlphaR = std::max(0.0f, wr.dotProduct(wo));

		brdfValue = kd + ks * (pow(cosAlphaR, exponent) / cosTheta);
	}
	// Else: brdf value is (0,0,0).

	return irradiance * brdfValue * cosTheta;
}

Vec3f ModifiedPhongBRDF::getBRDFValue(const Hit& hit, const Material& material, const Vec3f& wi, const Vec3f& wo, const Vec3f& irradiance) const
{
	Vec3f brdfValue = Vec3f();
	float cosTheta = std::max(0.0f, wi.dotProduct(hit.normal));

	if (cosTheta > 0.0f)
	{
		// theta < 90°
		Vec3f kd = material.diffuse;
		Vec3f ks = material.specular;

		Vec3f wr = (2 * (hit.normal * (wi.dotProduct(hit.normal))) - wi).unitVector();
		float cosAlphaR = std::max(0.0f, wr.dotProduct(wo));

		if (normalized == true)
		{
			brdfValue = (kd / PI) + (ks * ((exponent + 2) / (2 * PI)) * pow(cosAlphaR, exponent));
		}
		else
		{
			brdfValue = kd + ks * pow(cosAlphaR, exponent);
		}
	}
	// Else: brdf value is (0,0,0).

	return irradiance * brdfValue * cosTheta;
}

Vec3f BlinnPhongBRDF::getBRDFValue(const Hit& hit, const Material& material, const Vec3f& wi, const Vec3f& wo, const Vec3f& irradiance) const
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

		brdfValue = kd + ks * (pow(cosAlphaH, exponent) / cosTheta);
	}
	// Else: brdf value is (0,0,0).

	return irradiance * brdfValue * cosTheta;
}

Vec3f ModifiedBlinnPhongBRDF::getBRDFValue(const Hit& hit, const Material& material, const Vec3f& wi, const Vec3f& wo, const Vec3f& irradiance) const
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

		if (normalized == true)
		{
			brdfValue = (kd / PI) + (ks * ((exponent + 8) / (8 * PI)) * pow(cosAlphaH, exponent));
		}
		else
		{
			brdfValue = kd + ks * pow(cosAlphaH, exponent);
		}
	}
	// Else: brdf value is (0,0,0).

	return irradiance * brdfValue * cosTheta;
}