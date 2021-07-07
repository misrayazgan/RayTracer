#ifndef LIGHTMESH_H_
#define LIGHTMESH_H_

#include "Light.h"
#include "Mesh.h"

class LightMesh : public Light, public Mesh
{
public:
	Vec3f radiance;

	LightMesh(const Scene* scene_, int materialId_, Texture* texture_, Texture* normalTexture_, std::vector<Object*> triangles_,
		const Matrix4f& matrix_, bool transform_, const Vec3f& motionVector_, bool motion_, const Vec3f& radiance_);
	bool intersection(const Ray& ray, Hit& hit);
	Vec3f calculateWi(const Vec3f& intersectionPoint, const Vec3f& normal);
	float calculateDistance(const Vec3f& intersectionPoint);
	Vec3f calculateIrradiance(const Vec3f& intersectionPoint);
	void calculateCDF();

private:
	static thread_local Vec3f q;	// sampled point on light
	static thread_local Vec3f wi;
	static thread_local float pw;

	float totalArea;
	std::vector<float> cdf;
	std::default_random_engine randGenerator;
	std::uniform_real_distribution<float> distribution;
};


#endif
