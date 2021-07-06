#ifndef LIGHTSPHERE_H_
#define LIGHTSPHERE_H_

#include "Light.h"
#include "Sphere.h"

class LightSphere : public Light, public Sphere
{
public:
	Vec3f radiance;

	LightSphere(const Scene* scene_, const int center_, float radius_, int material_, Texture* texture_, Texture* normalTexture_,
		const Matrix4f& matrix_, bool transform_, const Vec3f& motionVector_, bool motion_, const Vec3f& radiance_);
	bool intersection(const Ray& ray, Hit& hit);
	Vec3f calculateWi(const Vec3f& intersectionPoint, const Vec3f& normal);
	float calculateDistance(const Vec3f& intersectionPoint);
	Vec3f calculateIrradiance(const Vec3f& intersectionPoint);

private:
	static thread_local Vec3f pointOnLight;
	static thread_local Vec3f wi;
	static thread_local float pw;

	std::default_random_engine randGenerator;
	std::uniform_real_distribution<float> distribution;
};


#endif
