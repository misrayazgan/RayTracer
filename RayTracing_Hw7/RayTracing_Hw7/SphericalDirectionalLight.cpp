#include "Light.h"

// Each thread works on separate l objects.
thread_local Vec3f SphericalDirectionalLight::l;

SphericalDirectionalLight::SphericalDirectionalLight(const std::string& imagePath_)
{
	environmentMap = new ImageTexture(imagePath_, "nearest", "replace_kd", 255.0f, 1.0f);

	// Initialize uniform random distribution to sample points inside the spherical light later.
	randGenerator.seed(std::chrono::system_clock::now().time_since_epoch().count());
	distribution = std::uniform_real_distribution<float>(0.0f, 1.0f);
}

Vec3f SphericalDirectionalLight::calculateWi(const Vec3f& intersectionPoint, const Vec3f& normal)
{
	// Uniform Random Rejection Sampling
	while (true)
	{
		// Sample points in [0,1] range.
		l.x = distribution(randGenerator);
		l.y = distribution(randGenerator);
		l.z = distribution(randGenerator);

		// Bring them to [-1,1] range.
		l.x = 2.0f * l.x - 1.0f;
		l.y = 2.0f * l.y - 1.0f;
		l.z = 2.0f * l.z - 1.0f;

		if (l.length() <= 1.0f && (l.dotProduct(normal) > 0.0f))
		{
			l = l.unitVector();
			break;
		}
	}
	
	return l;
}

float SphericalDirectionalLight::calculateDistance(const Vec3f& intersectionPoint)
{
	float dist = std::numeric_limits<float>::max();
	return dist;
}

Vec3f SphericalDirectionalLight::calculateIrradiance(const Vec3f& intersectionPoint)
{
	// l is already computed when calculateWi was called.
	Vec3f L = getTextureColor(l);
	return L * (2.0f * PI);
}

Vec3f SphericalDirectionalLight::getTextureColor(const Vec3f& lDir)
{
	float theta = acos(lDir.y);
	float phi = atan2(lDir.z, lDir.x);
	float u = (PI - phi) / (2.0f * PI);
	float v = theta / PI;

	Vec3f color = environmentMap->getTextureColor(Vec2f(u, v), Vec3f());
	return color;
}

