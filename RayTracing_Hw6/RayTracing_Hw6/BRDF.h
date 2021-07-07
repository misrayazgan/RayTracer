#ifndef BRDF_H_
#define BRDF_H_

#include "Vec3f.h"
#include "Ray.h"
#include "Material.h"

class BRDF
{
public:
	virtual Vec3f getBRDFValue(const Hit& hit, const Material& material, const Vec3f& wi, const Vec3f& wo, const Vec3f& irradiance) const = 0;
};

class PhongBRDF : public BRDF
{
public:
	PhongBRDF(float exponent_) : exponent(exponent_) {}
	Vec3f getBRDFValue(const Hit& hit, const Material& material, const Vec3f& wi, const Vec3f& wo, const Vec3f& irradiance) const;

private:
	float exponent;
};

class ModifiedPhongBRDF : public BRDF
{
public:
	ModifiedPhongBRDF(float exponent_, bool normalized_) : exponent(exponent_), normalized(normalized_) {}
	Vec3f getBRDFValue(const Hit& hit, const Material& material, const Vec3f& wi, const Vec3f& wo, const Vec3f& irradiance) const;

private:
	float exponent;
	bool normalized;
};

class BlinnPhongBRDF : public BRDF
{
public:
	BlinnPhongBRDF(float exponent_) : exponent(exponent_) {}
	Vec3f getBRDFValue(const Hit& hit, const Material& material, const Vec3f& wi, const Vec3f& wo, const Vec3f& irradiance) const;

private:
	float exponent;
};

class ModifiedBlinnPhongBRDF : public BRDF
{
public:
	ModifiedBlinnPhongBRDF(float exponent_, bool normalized_) : exponent(exponent_), normalized(normalized_) {}
	Vec3f getBRDFValue(const Hit& hit, const Material& material, const Vec3f& wi, const Vec3f& wo, const Vec3f& irradiance) const;

private:
	float exponent;
	bool normalized;
};

class TorranceSparrowBRDF : public BRDF
{
public:
	TorranceSparrowBRDF(float exponent_, bool kdfresnel_) : exponent(exponent_), kdfresnel(kdfresnel_) {}
	Vec3f getBRDFValue(const Hit& hit, const Material& material, const Vec3f& wi, const Vec3f& wo, const Vec3f& irradiance) const;

private:
	float exponent;
	bool kdfresnel;

	float blinnDistribution(float cosAlpha) const;
	float geometryTerm(const Vec3f& wi, const Vec3f& wo, const Vec3f& wh, const Vec3f& n) const;
	float fresnelReflectance(float cosBeta, const Material& material) const;
};

#endif
