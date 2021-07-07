#ifndef MATERIAL_H_
#define MATERIAL_H_

#include "Vec3f.h"

enum MaterialType
{
	MATERIALTYPE_NULL = 0,
	MATERIALTYPE_MIRROR,
	MATERIALTYPE_CONDUCTOR,
	MATERIALTYPE_DIELECTRIC
};

struct Material
{
    Vec3f ambient;
    Vec3f diffuse;
    Vec3f specular;
    Vec3f mirror;
	Vec3f absorption;
    float phongExponent;
	float refractionIndex;
	float absorptionIndex;
	float roughness;
	MaterialType type;
	bool diffuseExists;
	bool specularExists;
	bool roughnessExists;
	bool degamma;
};

#endif