#include "LightMesh.h"
#include "BVH.h"
#include "Triangle.h"
#include "Scene.h"

// Each thread works on separate q, wi and pw objects.
thread_local Vec3f LightMesh::q;
thread_local Vec3f LightMesh::wi;
thread_local float LightMesh::pw;

LightMesh::LightMesh(const Scene* scene_, int materialId_, Texture* texture_, Texture* normalTexture_, std::vector<Object*> triangles_,
	const Matrix4f& matrix_, bool transform_, const Vec3f& motionVector_, bool motion_, const Vec3f& radiance_)
	: Mesh(scene_, materialId_, texture_, normalTexture_, triangles_, matrix_, transform_, motionVector_, motion_), radiance(radiance_)
{
	// Initialize CDF for mesh triangles.
	calculateCDF();

	// Initialize uniform random distribution to sample points on the surface of the area light later.
	randGenerator.seed(std::chrono::system_clock::now().time_since_epoch().count());
	distribution = std::uniform_real_distribution<float>(0.0f, 1.0f);
}

void LightMesh::calculateCDF()
{
	// Calculate CDF for triangles.
	// It will be used to select triangles with a probability proportional to their areas.
	totalArea = 0.0f;

	for (int i = 0; i < triangles.size(); i++)
	{
		Triangle* triangle = dynamic_cast<Triangle*>(triangles[i]);
		float area = triangle->getArea(transformationMatrix);
		totalArea += area;
		cdf.push_back(totalArea);
	}
	
	// Divide by the total area, so that all elements are between 0 and 1 (used as probabilities).
	for (int i = 0; i < cdf.size(); i++)
	{
		cdf[i] /= totalArea;
	}
}

Vec3f LightMesh::calculateWi(const Vec3f& intersectionPoint, const Vec3f& normal)
{
	// Select a triangle randomly.
	float rand = distribution(randGenerator);
	Triangle* triangle = NULL;

	for (int i = 0; i < cdf.size(); i++)
	{
		if (rand <= cdf[i])
		{
			triangle = dynamic_cast<Triangle*>(triangles[i]);
			break;
		}
	}

	// Transform vertices of the triangle.
	Vec3f a = scene->vertexData[triangle->v0].position;
	Vec3f b = scene->vertexData[triangle->v1].position;
	Vec3f c = scene->vertexData[triangle->v2].position;

	Vec3f aNew = transformationMatrix.multiplyWithPoint(a);
	Vec3f bNew = transformationMatrix.multiplyWithPoint(b);
	Vec3f cNew = transformationMatrix.multiplyWithPoint(c);

	// Sample a uniform random point on the selected triangle.
	float e1 = distribution(randGenerator);
	float e2 = distribution(randGenerator);
	Vec3f p = (1.0f - e2) * bNew + e2 * cNew;
	q = sqrt(e1) * p + (1.0f - sqrt(e1)) * aNew;

	// Calculate p(w)
	float rSquare = (q - intersectionPoint).lengthSquared();
	float cosTheta = std::max(0.001f, triangle->normal.dotProduct(intersectionPoint - q));
	pw = rSquare / (totalArea * cosTheta);

	wi = (q - intersectionPoint).unitVector();
	return wi;
}

float LightMesh::calculateDistance(const Vec3f& intersectionPoint)
{
	float dist = (q - scene->shadowRayEpsilon * wi - intersectionPoint).length();
	return dist;
}

Vec3f LightMesh::calculateIrradiance(const Vec3f& intersectionPoint)
{
	Vec3f irradiance = radiance / pw;
	return irradiance;
}

bool LightMesh::intersection(const Ray& ray, Hit& hit)
{
	bool result = Mesh::intersection(ray, hit);
	hit.isLight = true;
	hit.radiance = radiance;
	hit.lightObject = this;

	return result;
}