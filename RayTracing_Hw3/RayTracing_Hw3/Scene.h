#ifndef SCENE_H_
#define SCENE_H_

#include "Vec3f.h"
#include "Vec3i.h"
#include "Vertex.h"
#include <vector>
#include <cmath>
#include <random>
#include "tinyxml2\tinyxml2.h"
#include "happly\happly.h"
#include "lodepng\lodepng.h"
#include "Camera.h"
#include "PointLight.h"
#include "AreaLight.h"
#include "Material.h"
#include "Mesh.h"
#include "MeshInstance.h"
#include "Sphere.h"
#include "Ray.h"
#include "Transformation.h"

class Scene
{
public:
	Vec3i backgroundColor;
	float shadowRayEpsilon;
	float testEpsilon;
	int maxRecursionDepth;
	Vec3f ambientLight;
	Object* bvh;

	std::vector<Camera> cameras;
	std::vector<PointLight> pointLights;
	std::vector<AreaLight> areaLights;
	std::vector<Material> materials;
    std::vector<Vertex> vertexData;
    std::vector<Mesh*> baseMeshes;
	std::vector<Translation> translations;
	std::vector<Scaling> scalings;
	std::vector<Rotation> rotations;

	void loadSceneFromXml(const std::string& filepath);
	void parsePlyFile(const std::string& filepath, const std::string& plyFile, std::vector<Object*>& triangles, ShadingMode shadingMode, int materialId);
	void renderScene();
	void renderScenePartial(int minHeight, int maxHeight, int cameraIdx, unsigned char* image, std::default_random_engine& randGenerator, std::uniform_real_distribution<float>& distribution);
	Vec3f renderPixel(const Camera& camera, int i, int j, std::default_random_engine& randGenerator, std::uniform_real_distribution<float>& distribution);
	Ray generateRay(const Camera& camera, int i, int j, float time, float dx = 0.5f, float dy = 0.5f);
	Ray generateRayDepthOfField(const Camera& camera, int i, int j, float dx, float dy, float dofRandx, float dofRandy, float time);
	Vec3f findPixelColor(const Ray& ray, const Camera& camera, int depth, std::default_random_engine& randGenerator, std::uniform_real_distribution<float>& distribution);
	bool refractRay(Vec3f direction, Vec3f normal, float n1, float n2, Vec3f& wt);
	float findReflectionRatioDielectric(float cosTheta, float n1, float n2);
	float findReflectionRatioConductor(float cosTheta, float n1, float n2);
	Vec3f renderPixelMultisampling(const Camera& camera, int i, int j, std::default_random_engine& randGenerator, std::uniform_real_distribution<float>& distribution);
	std::vector<Ray> sampleRays(const Camera& camera, int i, int j, std::default_random_engine& randGenerator, std::uniform_real_distribution<float>& distribution);
	std::vector<Ray> sampleRaysDepthOfField(const Camera& camera, int i, int j, std::default_random_engine& randGenerator, std::uniform_real_distribution<float>& distribution);
	~Scene();

private:
	int clampPixel(float val);
	Vec3f diffuseShading(const PointLight& pointLight, const Vec3f& intersectionPoint, const Vec3f& normal, const Material& material);
	Vec3f diffuseShadingArea(const AreaLight& areaLight, const Vec3f& point, const Hit& hit, const Material& material);
	Vec3f specularShading(const PointLight& pointLight, const Vec3f& intersectionPoint, const Vec3f& normal, const Material& material, const Ray& ray);
	Vec3f specularShadingArea(const AreaLight& areaLight, const Vec3f& point, const Hit& hit, const Material& material, const Ray& ray);
	Vec3f calculateIrradiance(const PointLight& pointLight, const Vec3f& intersectionPoint);
	void applyTransformations(tinyxml2::XMLElement* element, std::stringstream& stream, Matrix4f& matrix);
};


#endif