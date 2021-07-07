#include "Scene.h"
#include "Utils.h"
#include <sstream>
#include <stdexcept>
#include <iostream>
#include <thread>

void Scene::renderScene()
{
	std::default_random_engine randGenerator;
	randGenerator.seed(std::chrono::system_clock::now().time_since_epoch().count());
	std::uniform_real_distribution<float> distribution(0.0f, 1.0f);

	int numberOfThreads = 8;
	int numberOfCameras = cameras.size();

	for (int i = 0; i < numberOfCameras; i++)
	{
		Camera currentCamera = cameras[i];
		int width = currentCamera.imageWidth;
		int height = currentCamera.imageHeight;

		auto start = std::chrono::system_clock::now();
		std::vector<Vec3f> pixelColors(width * height, Vec3f());

		int stepSize = height / numberOfThreads;
		std::vector<std::thread> threads;
		for (int k = 0; k < numberOfThreads; k++)
		{
			int minHeight = k * stepSize;
			int maxHeight = minHeight + stepSize;

			threads.push_back(std::thread(&Scene::renderScenePartial, this, minHeight, maxHeight, i, std::ref(pixelColors), std::ref(randGenerator), std::ref(distribution)));
		}

		for (int i = 0; i < numberOfThreads; i++)
		{
			threads[i].join();
		}

		auto end = std::chrono::system_clock::now();
		printTimeDuration(start, end);

		Image image = Image(width, height, pixelColors);
		image.writeImage(currentCamera.imageName, currentCamera.tonemap);
	}
}

void Scene::renderScenePartial(int minHeight, int maxHeight, int cameraIdx, std::vector<Vec3f>& pixelColors, std::default_random_engine& randGenerator, std::uniform_real_distribution<float>& distribution)
{
	Camera camera = cameras[cameraIdx];
	camera.setCameraParams();

	int width = camera.imageWidth;
	int rgbIdx = minHeight * width;

	if (camera.numberOfSamples == 1)
	{
		for (int i = minHeight; i < maxHeight; i++)
		{
			for (int j = 0; j < width; j++)
			{
				Vec3f color = renderPixel(camera, j, i, randGenerator, distribution);
				pixelColors[rgbIdx++] = color;
			}
		}
	}
	else
	{
		// Ray tracing with multisampling
		for (int i = minHeight; i < maxHeight; i++)
		{
			for (int j = 0; j < width; j++)
			{
				Vec3f color = renderPixelMultisampling(camera, j, i, randGenerator, distribution);
				pixelColors[rgbIdx++] = color;
			}
		}
	}	
}

Vec3f Scene::renderPixelMultisampling(const Camera& camera, int i, int j, std::default_random_engine& randGenerator, std::uniform_real_distribution<float>& distribution)
{
	// Box Filtering
	Vec3f color = Vec3f();
	std::vector<Ray> raysPerPixel;

	if (camera.hasDepthOfField() == true)
	{
		raysPerPixel = sampleRaysDepthOfField(camera, i, j, randGenerator, distribution);
	}
	else
	{
		raysPerPixel = sampleRays(camera, i, j, randGenerator, distribution);
	}
	
	for (int r = 0; r < raysPerPixel.size(); r++)
	{
		Ray ray = raysPerPixel[r];
		color += findPixelColor(ray, camera, maxRecursionDepth, i, j, randGenerator, distribution);
	}

	color = color / camera.numberOfSamples;

	return color;
}

std::vector<Ray> Scene::sampleRays(const Camera& camera, int i, int j, std::default_random_engine& randGenerator, std::uniform_real_distribution<float>& distribution)
{
	// Jittered Multisampling
	// Sample rays for a single pixel.
	std::vector<Ray> raysPerPixel;
	int numberOfMiniPixels = sqrt(camera.numberOfSamples);

	for (int a = 0; a < numberOfMiniPixels; a++)
	{
		for (int b = 0; b < numberOfMiniPixels; b++)
		{
			float randx = distribution(randGenerator);
			float randy = distribution(randGenerator);

			float dx = (a + randx) / numberOfMiniPixels;
			float dy = (b + randy) / numberOfMiniPixels;

			float time = distribution(randGenerator);

			Ray ray = generateRay(camera, i, j, time, dx, dy);
			raysPerPixel.push_back(ray);
		}
	}
	
	return raysPerPixel;
}

std::vector<Ray> Scene::sampleRaysDepthOfField(const Camera& camera, int i, int j, std::default_random_engine& randGenerator, std::uniform_real_distribution<float>& distribution)
{
	// Sample rays for a single pixel with depth of field camera.
	std::vector<Ray> raysPerPixel;
	int numberOfMiniPixels = sqrt(camera.numberOfSamples);

	for (int a = 0; a < numberOfMiniPixels; a++)
	{
		for (int b = 0; b < numberOfMiniPixels; b++)
		{
			float randx = distribution(randGenerator);
			float randy = distribution(randGenerator);

			float dx = (a + randx) / numberOfMiniPixels;
			float dy = (b + randy) / numberOfMiniPixels;

			// Distribution is between 0 and 1, subtract 0.5 because center of the aperture is used.
			float dofRandx = distribution(randGenerator) - 0.5f;
			float dofRandy = distribution(randGenerator) - 0.5f;

			float time = distribution(randGenerator);

			Ray ray = generateRayDepthOfField(camera, i, j, dx, dy, dofRandx, dofRandy, time);
			raysPerPixel.push_back(ray);
		}
	}

	return raysPerPixel;
}

Vec3f Scene::renderPixel(const Camera& camera, int i, int j, std::default_random_engine& randGenerator, std::uniform_real_distribution<float>& distribution)
{
	float time = distribution(randGenerator);
	Ray ray = generateRay(camera, i, j, time);
	Vec3f color = findPixelColor(ray, camera, maxRecursionDepth, i, j, randGenerator, distribution);

	return color;
}

Ray Scene::generateRay(const Camera& camera, int i, int j, float time, float dx, float dy)
{
	// dx and dy are used for multisampling, otherwise they are set to 0.5 by default.
	// i -> index along width
	// j -> index along height
	float su = (camera.right - camera.left)*(i + dx)/(float)camera.imageWidth;
	float sv = (camera.top - camera.bottom)*(j + dy)/(float)camera.imageHeight;

	Vec3f s = camera.q + su*camera.u - sv*camera.v;

	Ray ray = Ray();
	ray.origin = camera.position;
	ray.direction = (s - camera.position).unitVector();
	ray.time = time;

	return ray;
}

Ray Scene::generateRayDepthOfField(const Camera& camera, int i, int j, float dx, float dy, float dofRandx, float dofRandy, float time)
{
	// Sample point in a pixel.
	float su = (camera.right - camera.left)*(i + dx) / (float)camera.imageWidth;
	float sv = (camera.top - camera.bottom)*(j + dy) / (float)camera.imageHeight;
	Vec3f pixelSample = camera.q + su * camera.u - sv * camera.v;

	// Sample point on aperture.
	Vec3f apertureSample = camera.position + camera.apertureSize * (dofRandx * camera.u + dofRandy * camera.v);

	Vec3f dir = (pixelSample - camera.position).unitVector();
	float tFocusDistance = camera.focusDistance / (dir.dotProduct(camera.gaze));
	Ray ray = Ray(camera.position, dir, time);

	Vec3f pointOnFocalPlane = ray.pointAtParam(tFocusDistance);	
	Vec3f d = (pointOnFocalPlane - apertureSample).unitVector();
	Ray primaryRay(apertureSample, d, time);

	return primaryRay;
}

Vec3f Scene::findPixelColor(const Ray& ray, const Camera& camera, int depth, int i, int j, std::default_random_engine& randGenerator, std::uniform_real_distribution<float>& distribution)
{
	Vec3f color = Vec3f();

	Hit hitResult;
	hitResult.t = kInf;
	bool result = bvh->intersection(ray, hitResult);

	if (result == true)
	{
		Material material = materials[hitResult.materialId];
		Texture *texture = hitResult.texture;

		if (texture && texture->getDecalMode() == DECALMODE_REPLACEALL)
		{
			// If mode is replace_all, disable all shading and directly paste the texture color.
			color = texture->getTextureColor(hitResult.uvTexture, hitResult.intersectionPoint);
			return color;
		}

		applyDegamma(material, camera.tonemap);
			
		// Do not add shading if ray is inside an object.
		if (ray.isInsideObject == false)
		{
			color += material.ambient * ambientLight;

			// For each light in the scene, add diffuse and specular shading to the pixel color.
			for (int i = 0; i < lights.size(); i++)
			{
				Light *currentLight = lights[i];

				/**********Shadow check**********/
				Vec3f wi = currentLight->calculateWi(hitResult.intersectionPoint, hitResult.normal);
				Ray shadowRay = Ray();
				shadowRay.origin = hitResult.intersectionPoint + shadowRayEpsilon * hitResult.normal;
				shadowRay.direction = wi;
				shadowRay.time = ray.time;

				Hit shadowHit;
				shadowHit.t = kInf;
				bool shadowResult = bvh->intersection(shadowRay, shadowHit);

				float tLight = currentLight->calculateDistance(hitResult.intersectionPoint);
				bool shadow = false;
				if (shadowResult == true && shadowHit.t < tLight - testEpsilon && shadowHit.t > 0.0f)
				{
					shadow = true;
				}

				if (shadow == false)
				{
					Vec3f irradiance = currentLight->calculateIrradiance(hitResult.intersectionPoint);

					if (material.brdfId > -1)
					{
						// BRDF exists
						BRDF *brdf = brdfs[material.brdfId];
						Vec3f wo = (ray.origin - hitResult.intersectionPoint).unitVector();
						color += brdf->getBRDFValue(hitResult, material, wi, wo, irradiance);
					}
					else
					{
						Vec3f diffuse = diffuseShading(irradiance, wi, hitResult, material, texture);
						Vec3f specular = specularShading(irradiance, wi, hitResult, material, ray);
						color += diffuse + specular;
					}
				}
			}
		}

		// Specular Reflection
		if (material.type == MATERIALTYPE_MIRROR && depth > 0)
		{
			Vec3f wo = (ray.origin - hitResult.intersectionPoint).unitVector();
			Vec3f wr = (2 * (hitResult.normal * (wo.dotProduct(hitResult.normal))) - wo).unitVector();

			if (material.roughnessExists == true)
			{
				// Construct orthonormal basis uvr.
				Vec3f rPrime = wr;
				int minIdx = rPrime.getAbsMinElementIndex();
				rPrime[minIdx] = 1.0f;

				Vec3f u = wr.crossProduct(rPrime).unitVector();
				Vec3f v = wr.crossProduct(u).unitVector();

				float randu = distribution(randGenerator) - 0.5f;
				float randv = distribution(randGenerator) - 0.5f;
				wr = (wr + material.roughness * (randu * u + randv * v));
			}

			Ray mirrorRay = Ray();
			mirrorRay.origin = hitResult.intersectionPoint + shadowRayEpsilon * hitResult.normal;
			mirrorRay.direction = wr;
			mirrorRay.time = ray.time;

			color += material.mirror * findPixelColor(mirrorRay, camera, depth - 1, i, j, randGenerator, distribution);			
		}

		// Refraction
		if (material.type == MATERIALTYPE_DIELECTRIC && depth > 0)
		{
			Vec3f wo = (ray.origin - hitResult.intersectionPoint).unitVector();
			Vec3f wr = (2 * (hitResult.normal * (wo.dotProduct(hitResult.normal))) - wo).unitVector();
			Vec3f wt = Vec3f();

			float n1 = 1.0f;
			float n2 = material.refractionIndex;
			Vec3f transparency = Vec3f();
			
			float cosTheta = -1 * ray.direction.dotProduct(hitResult.normal);
			bool entering = cosTheta > 0.0f;

			if (entering)
			{
				// Entering ray
				bool refractionResult = refractRay(ray.direction, hitResult.normal, n1, n2, wt);
				transparency = Vec3f(1.0f, 1.0f, 1.0f);
			}
			else
			{
				// Exiting ray
				cosTheta = std::abs(cosTheta);
				std::swap(n1, n2);

				bool refractionResult = refractRay(ray.direction, -1 * hitResult.normal, n1, n2, wt);
				transparency.x = expf(-1 * material.absorption.x * hitResult.t);
				transparency.y = expf(-1 * material.absorption.y * hitResult.t);
				transparency.z = expf(-1 * material.absorption.z * hitResult.t);
			}

			float fr = findReflectionRatioDielectric(cosTheta, n1, n2);
			float ft = 1 - fr;

			if (fr == 1.0f)
			{
				// Total Internal Reflection happened
				Ray reflectionRay = Ray();
				reflectionRay.origin = hitResult.intersectionPoint + shadowRayEpsilon * wr;
				reflectionRay.direction = wr;
				reflectionRay.isInsideObject = true;
				reflectionRay.time = ray.time;

				Vec3f reflectionColor = findPixelColor(reflectionRay, camera, depth - 1, i, j, randGenerator, distribution);				
				color += transparency * reflectionColor;
			}
			else
			{
				Ray refractionRay = Ray();
				refractionRay.origin = hitResult.intersectionPoint + shadowRayEpsilon * wt;
				refractionRay.direction = wt;
				refractionRay.time = ray.time;
				// If entering ray, refraction is inside. Else, outside.
				refractionRay.isInsideObject = entering;

				Vec3f refractionColor = findPixelColor(refractionRay, camera, depth - 1, i, j, randGenerator, distribution);

				Ray reflectionRay = Ray();
				reflectionRay.origin = hitResult.intersectionPoint + shadowRayEpsilon * wr;
				reflectionRay.direction = wr;
				reflectionRay.time = ray.time;
				// If entering ray, reflection is outside. Else, inside.
				reflectionRay.isInsideObject = !entering;

				Vec3f reflectionColor = findPixelColor(reflectionRay, camera, depth - 1, i, j, randGenerator, distribution);				
				color += transparency * ((fr * reflectionColor) + (refractionColor * ft));
			}
		}

		if (material.type == MATERIALTYPE_CONDUCTOR && depth > 0)
		{
			Vec3f wo = (ray.origin - hitResult.intersectionPoint).unitVector();
			Vec3f wr = (2 * (hitResult.normal * (wo.dotProduct(hitResult.normal))) - wo).unitVector();

			if (material.roughnessExists == true)
			{
				// Construct orthonormal basis uvr.
				Vec3f rPrime = wr;
				int minIdx = rPrime.getAbsMinElementIndex();
				rPrime[minIdx] = 1.0f;

				Vec3f u = wr.crossProduct(rPrime).unitVector();
				Vec3f v = wr.crossProduct(u).unitVector();

				float randu = distribution(randGenerator) - 0.5f;
				float randv = distribution(randGenerator) - 0.5f;
				wr = (wr + material.roughness * (randu * u + randv * v));
			}
			
			float cosTheta = -1 * ray.direction.dotProduct(hitResult.normal);
			float fr = findReflectionRatioConductor(cosTheta, material.refractionIndex, material.absorptionIndex);

			Ray reflectionRay = Ray();
			reflectionRay.origin = hitResult.intersectionPoint + shadowRayEpsilon * hitResult.normal;
			reflectionRay.direction = wr;
			reflectionRay.time = ray.time;

			Vec3f reflectionColor = findPixelColor(reflectionRay, camera, depth - 1, i, j, randGenerator, distribution);
			color += material.mirror * fr * reflectionColor;	
		}
	}
	else
	{
		// No hit happened
		if (maxRecursionDepth == depth)
		{
			// Primary ray, pixel's color is background color or background texture.
			color = getBackgroundColor(i, j, ray);
		}
		// If ray is not primary ray, pixel's color is (0,0,0).
	}

	return color;
}

Vec3f Scene::getBackgroundColor(int i, int j, const Ray& ray) const
{
	Vec3f color = Vec3f();

	if (backgroundTexture)
	{
		i = i % backgroundTexture->width;
		j = j % backgroundTexture->height;

		Vec2f uv;
		uv.x = i / (float)backgroundTexture->width;
		uv.y = j / (float)backgroundTexture->height;
		color = backgroundTexture->getTextureColor(uv, Vec3f());
		//color = backgroundTexture->fetch(i, j);
	}
	else if (sphericalDirLight)
	{
		color = sphericalDirLight->getTextureColor(ray.direction);
	}
	else
	{
		color = backgroundColor;
	}

	return color;
}

Vec3f Scene::diffuseShading(const Vec3f& irradiance, const Vec3f& wi, const Hit& hit, const Material& material, const Texture* texture)
{
	Vec3f diffuse = Vec3f();
	Vec3f kd = material.diffuse;

	// Handle texture
	if (texture && texture->isNormalMap == false)
	{
		Vec3f textureColor = texture->getTextureColor(hit.uvTexture, hit.intersectionPoint);

		if (texture->getDecalMode() == DECALMODE_REPLACEKD)
		{
			kd = textureColor;
		}
		else if (texture->getDecalMode() == DECALMODE_BLENDKD)
		{
			kd = (kd + textureColor) / 2.0f;
		}
	}

	float cosTheta = std::max(0.0f, wi.dotProduct(hit.normal));
	diffuse = kd * cosTheta * irradiance;

	return diffuse;
}

Vec3f Scene::specularShading(const Vec3f& irradiance, const Vec3f& wi, const Hit& hit, const Material& material, const Ray& ray)
{
	Vec3f specular = Vec3f();
	if (material.specularExists == true)
	{
		Vec3f wo = (ray.origin - hit.intersectionPoint).unitVector();
		Vec3f h = (wi + wo).unitVector();
		float cosAlpha = std::max(0.0f, hit.normal.dotProduct(h));

		specular = material.specular * powf(cosAlpha, material.phongExponent) * irradiance;
	}	

	return specular;
}

bool Scene::refractRay(Vec3f direction, Vec3f normal, float n1, float n2, Vec3f& wt)
{
	bool result = false;
	float cosTheta = -1 * direction.dotProduct(normal);
	float insideTerm = 1 - (n1 / n2) * (n1 / n2) * (1 - cosTheta * cosTheta);

	if (insideTerm >= 0.0f)
	{
		float cosPhi = sqrt(insideTerm);
		wt = ((direction + normal * cosTheta) * (n1 / n2) - normal * cosPhi).unitVector();
		result = true;
	}
	// If inside of the squareroot is negative, refraction does not happen.

	return result;
}

float Scene::findReflectionRatioDielectric(float cosTheta, float n1, float n2)
{
	// Compute cosPhi using Snell's law
	float sinTheta = sqrt(std::max(0.0f, 1 - cosTheta * cosTheta));
	float sinPhi = (n1 / n2) * sinTheta;
	if (sinPhi >= 1.0f)
	{
		// Total Internal Reflection: no refraction happens, all reflection
		return 1.0f;
	}

	float cosPhi = sqrt(std::max(0.0f, 1 - sinPhi * sinPhi));

	// Fresnel equations for dielectrics
	float rs = (n2 * cosTheta - n1 * cosPhi) / (n2 * cosTheta + n1 * cosPhi);
	float rp = (n1 * cosTheta - n2 * cosPhi) / (n1 * cosTheta + n2 * cosPhi);

	float fr = (rs * rs + rp * rp) / 2;
	return fr;
}

float Scene::findReflectionRatioConductor(float cosTheta, float n, float k)
{
	// Fresnel equations for conductors
	float rs = ((n*n + k*k) - 2*n*cosTheta + cosTheta*cosTheta) / 
			   ((n*n + k*k) + 2*n*cosTheta + cosTheta*cosTheta);
	float rp = ((n*n + k*k)*cosTheta*cosTheta - 2*n*cosTheta + 1) / 
			   ((n*n + k*k)*cosTheta*cosTheta + 2*n*cosTheta + 1);
	
	float fr = (rs + rp) / 2;
	return fr;
}

void Scene::applyDegamma(Material& material, const Tonemap& tonemap)
{
	if (tonemap.tonemapOperator != TMO_NONE && material.degamma == true)
	{
		// Apply degamma to material
		material.ambient = material.ambient.pow(tonemap.gamma);
		material.diffuse = material.diffuse.pow(tonemap.gamma);
		material.specular = material.specular.pow(tonemap.gamma);
		material.mirror = material.mirror.pow(tonemap.gamma);
	}
}

Scene::~Scene()
{
	if (bvh)
	{
		delete bvh;
	}

	if (backgroundTexture)
	{
		delete backgroundTexture;
	}

	if (sphericalDirLight)
	{
		delete sphericalDirLight;
	}
}