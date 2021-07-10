#ifndef SCENE_H_
#define SCENE_H_

#include "Vec2f.h"
#include "Vec3f.h"
#include "Vertex.h"
#include <vector>
#include <cmath>
#include <random>
#include "tinyxml2\tinyxml2.h"
#include "happly\happly.h"
#include "Image.h"
#include "Camera.h"
#include "Light.h"
#include "Material.h"
#include "Mesh.h"
#include "MeshInstance.h"
#include "Sphere.h"
#include "Ray.h"
#include "BVH.h"
#include "Transformation.h"
#include "ImageTexture.h"
#include "PerlinTexture.h"
#include "CheckerboardTexture.h"
#include "Tonemap.h"
#include "BRDF.h"
#include "LightMesh.h"
#include "LightSphere.h"

class Scene
{
public:
	Vec3f backgroundColor;
	float shadowRayEpsilon;
	float testEpsilon;
	int maxRecursionDepth;
	Vec3f ambientLight;
	Object* bvh;
	ImageTexture* backgroundTexture;
	SphericalDirectionalLight* sphericalDirLight;

	std::vector<Camera> cameras;
	std::vector<Light*> lights;
	std::vector<Material> materials;
    std::vector<Vertex> vertexData;
    std::vector<Mesh*> baseMeshes;
	std::vector<Translation> translations;
	std::vector<Scaling> scalings;
	std::vector<Rotation> rotations;
	std::vector<Composite> composites;
	std::vector<Texture*> textures;
	std::vector<Vec2f> textureCoordData;
	std::vector<BRDF*> brdfs;

	// Parser
	void loadSceneFromXml(const std::string& filepath);
	
	void renderScene();
	void renderScenePartial(int minHeight, int maxHeight, int cameraIdx, std::vector<Vec3f>& pixelColors);
	Vec3f renderPixel(const Camera& camera, int i, int j);
	Vec3f renderPixelMultisampling(const Camera& camera, int i, int j);
	Ray generateRay(const Camera& camera, int i, int j, float time, float dx = 0.5f, float dy = 0.5f);
	Ray generateRayDepthOfField(const Camera& camera, int i, int j, float dx, float dy, float dofRandx, float dofRandy, float time);
	bool refractRay(Vec3f direction, Vec3f normal, float n1, float n2, Vec3f& wt);
	float findReflectionRatioDielectric(float cosTheta, float n1, float n2);
	float findReflectionRatioConductor(float cosTheta, float n1, float n2);
	std::vector<Ray> sampleRays(const Camera& camera, int i, int j);
	std::vector<Ray> sampleRaysDepthOfField(const Camera& camera, int i, int j);
	Vec3f findPixelColor(const Ray& ray, const Camera& camera, int depth, int i = 0, int j = 0);
	Vec3f findPixelColorPathTracing(const Ray& ray, const Camera& camera, int depth, int i = 0, int j = 0);
	~Scene();

private:
	std::default_random_engine randGenerator;
	std::uniform_real_distribution<float> distribution;

	bool shadowCheck(Light* light, const Ray& ray, const Hit& hitResult);
	Vec3f diffuseShading(const Vec3f& irradiance, const Vec3f& wi, const Hit& hit, const Material& material, const Texture* texture);
	Vec3f specularShading(const Vec3f& irradiance, const Vec3f& wi, const Hit& hit, const Material& material, const Ray& ray);
	Vec3f getReflectionColor(const Ray& ray, const Hit& hitResult, const Material& material, const Camera& camera, int depth);
	Vec3f getRefractionColor(const Ray& ray, const Hit& hitResult, const Material& material, const Camera& camera, int depth);
	Vec3f getBackgroundColor(int i, int j, const Ray& ray) const;
	void applyDegamma(Material& material, const Tonemap& tonemap);

	// Path Tracing
	Vec3f getDirectLightingColor(const Ray& ray, const Hit& hitResult, const Material& material, const Texture* texture);
	Vec3f getIndirectLightingColor(const Ray& ray, const Hit& hitResult, const Material& material, const Texture* texture, const Camera& camera, int depth, int i, int j);
	Vec3f sampleDirection(const Vec3f& normal, const Camera& camera);

	// Parser
	void applyTransformations(tinyxml2::XMLElement* element, std::stringstream& stream, Matrix4f& matrix);
	void parsePlyFile(const std::string& filepath, const std::string& plyFile, std::vector<Object*>& triangles,
		ShadingMode shadingMode, int materialId, Texture* texture, Texture* normalTexture, int vertexOffset, int textureOffset);
	void getRendererParams(tinyxml2::XMLElement* element, std::stringstream& stream, Camera& camera);
};


#endif