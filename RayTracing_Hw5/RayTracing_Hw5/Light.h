#ifndef LIGHT_H_
#define LIGHT_H_

#include "Vec3f.h"
#include "ImageTexture.h"
#include <random>
#include <chrono>

class Light
{
public:
	virtual Vec3f calculateWi(const Vec3f& intersectionPoint, const Vec3f& normal) = 0;
	virtual float calculateDistance(const Vec3f& intersectionPoint) = 0;
	virtual Vec3f calculateIrradiance(const Vec3f& intersectionPoint) = 0;
};

class PointLight : public Light
{
public:
	Vec3f position;
	Vec3f intensity;

	PointLight(const Vec3f& position_, const Vec3f& intensity_) : position(position_), intensity(intensity_) {}
	Vec3f calculateWi(const Vec3f& intersectionPoint, const Vec3f& normal);
	float calculateDistance(const Vec3f& intersectionPoint);
	Vec3f calculateIrradiance(const Vec3f& intersectionPoint);
};

class AreaLight : public Light
{
public:
	Vec3f position;
	Vec3f normal;
	float extent;
	Vec3f radiance;
	Vec3f u;
	Vec3f v;

	AreaLight(const Vec3f& position_, const Vec3f& normal_, float extent_, const Vec3f& radiance_);
	Vec3f calculateWi(const Vec3f& intersectionPoint, const Vec3f& normal);
	float calculateDistance(const Vec3f& intersectionPoint);
	Vec3f calculateIrradiance(const Vec3f& intersectionPoint);

private:
	static thread_local Vec3f wi;
	static thread_local Vec3f pointOnLight;
	std::default_random_engine randGenerator;
	std::uniform_real_distribution<float> distribution;
};

class DirectionalLight : public Light
{
public:
	Vec3f direction;
	Vec3f radiance;

	DirectionalLight(const Vec3f& direction_, const Vec3f& radiance_) : direction(direction_), radiance(radiance_) {}
	Vec3f calculateWi(const Vec3f& intersectionPoint, const Vec3f& normal);
	float calculateDistance(const Vec3f& intersectionPoint);
	Vec3f calculateIrradiance(const Vec3f& intersectionPoint);
};

class SpotLight : public Light
{
public:
	Vec3f position;
	Vec3f direction;
	Vec3f intensity;
	float alpha;	// coverage angle
	float beta;		// falloff angle

	SpotLight(const Vec3f& position_, const Vec3f& direction_, const Vec3f& intensity_, float cAngle_, float fAngle_)
		: position(position_), direction(direction_), intensity(intensity_), alpha(cAngle_), beta(fAngle_) {}
	Vec3f calculateWi(const Vec3f& intersectionPoint, const Vec3f& normal);
	float calculateDistance(const Vec3f& intersectionPoint);
	Vec3f calculateIrradiance(const Vec3f& intersectionPoint);

private:
	float calculateFalloff(const Vec3f& intersectionPoint);
};

class SphericalDirectionalLight : public Light
{
public:
	ImageTexture *environmentMap;

	SphericalDirectionalLight(const std::string& imagePath_);
	Vec3f calculateWi(const Vec3f& intersectionPoint, const Vec3f& normal);
	float calculateDistance(const Vec3f& intersectionPoint);
	Vec3f calculateIrradiance(const Vec3f& intersectionPoint);
	Vec3f getTextureColor(const Vec3f& lDir);

private:
	static thread_local Vec3f l;
	std::default_random_engine randGenerator;
	std::uniform_real_distribution<float> distribution;
};

#endif
