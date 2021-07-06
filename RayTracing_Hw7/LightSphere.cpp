#include "LightSphere.h"
#include "Scene.h"

// Each thread works on separate pointOnLight, wi and pw objects.
thread_local Vec3f LightSphere::pointOnLight;
thread_local Vec3f LightSphere::wi;
thread_local float LightSphere::pw;

LightSphere::LightSphere(const Scene* scene_, const int center_, float radius_, int material_, Texture* texture_, Texture* normalTexture_,
	const Matrix4f& matrix_, bool transform_, const Vec3f& motionVector_, bool motion_, const Vec3f& radiance_)
	: Sphere(scene_, center_, radius_, material_, texture_, normalTexture_, matrix_, transform_, motionVector_, motion_), radiance(radiance_)
{

}

Vec3f LightSphere::calculateWi(const Vec3f& intersectionPoint, const Vec3f& normal)
{
	// Transform the intersection point to local (sphere) coordinates.
	Vec3f localPoint = inverseTransformationMatrix.multiplyWithPoint(intersectionPoint);
	Vec3f center = scene->vertexData[centerVertexId].position;
	Vec3f w = center - localPoint;
	float d = w.length();

	float sinThetaMax = radius / d;
	float cosThetaMax = sqrt(1.0f - sinThetaMax * sinThetaMax);

	pw = 1.0f / (2.0f * PI * (1.0f - cosThetaMax));

	// Construct orthonormal basis uvw.
	w = w.unitVector();
	Vec3f wPrime = w;
	int minIdx = wPrime.getAbsMinElementIndex();
	wPrime[minIdx] = 1.0f;

	Vec3f u = wPrime.crossProduct(w).unitVector();
	Vec3f v = u.crossProduct(w).unitVector();

	// Compute phi and theta.
	float e1 = distribution(randGenerator);
	float e2 = distribution(randGenerator);

	float phi = 2.0f * PI * e1;
	float theta = acos(1.0f - e2 + e2 * cosThetaMax);

	Vec3f lLocal = w * cos(theta) + v * sin(theta) * cos(phi) + u * sin(theta) * sin(phi);
	Vec3f lWorld = transformationMatrix.multiplyWithVector(lLocal);

	// r(t) = pWorld + t.lWorld
	// Intersect r(t) with the lightSphere and find the point on the light from which light is obtained.
	Ray rayLight = Ray(intersectionPoint + scene->shadowRayEpsilon * lWorld.unitVector(), lWorld.unitVector());
	// origini kaydirmaya gerek var mi???????
	Hit hitLight;
	hitLight.t = kInf;
	bool result = Sphere::intersection(rayLight, hitLight);
	if (result == true)
	{
		pointOnLight = hitLight.intersectionPoint;
		wi = (hitLight.intersectionPoint - intersectionPoint).unitVector();
	}

	return wi;
}

float LightSphere::calculateDistance(const Vec3f& intersectionPoint)
{
	// Buralarda direk t donmek daha mi iyi olur bilemedim?????
	float dist = (pointOnLight - scene->shadowRayEpsilon * wi - intersectionPoint).length();
	return dist;
}

Vec3f LightSphere::calculateIrradiance(const Vec3f& intersectionPoint)
{
	Vec3f irradiance = radiance / pw;
	return irradiance;
}

bool LightSphere::intersection(const Ray& ray, Hit& hit)
{
	bool result = Sphere::intersection(ray, hit);
	hit.isLight = true;
	hit.radiance = radiance;

	return result;

	//return false;
}