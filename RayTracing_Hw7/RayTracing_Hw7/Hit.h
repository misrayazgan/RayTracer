#pragma once
#ifndef HIT_H_
#define HIT_H_

#include "Vec2f.h"
#include "Vec3f.h"

class Material;
class Texture;
class Light;

class Hit
{
public:
	int materialId;
	Vec3f intersectionPoint;
	Vec3f normal;
	float t;
	// Hit struct stores only shading texture, because shading related calculations are performed in the main ray tracing routine.
	// No need to store normal texture, because normal related calculations are performed in object-ray intersection routines.
	Texture* texture;
	Vec2f uvTexture;
	bool isLight;	// true if the object hit is a LightMesh or a LightSphere
	Vec3f radiance;	// radiance value of the object light
	Light *lightObject;	// set this if the object hit is a LightMesh or a LightSphere

	Hit() : isLight(false), radiance(Vec3f()), t(kInf), texture(NULL), lightObject(NULL) {}
};


#endif