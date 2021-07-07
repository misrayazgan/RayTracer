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

	void loadSceneFromXml(const std::string& filepath);
	void parsePlyFile(const std::string& filepath, const std::string& plyFile, std::vector<Object*>& triangles, 
		ShadingMode shadingMode, int materialId, Texture* texture, Texture* normalTexture, int vertexOffset, int textureOffset);
	void renderScene();
	void renderScenePartial(int minHeight, int maxHeight, int cameraIdx, std::vector<Vec3f>& pixelColors, std::default_random_engine& randGenerator, std::uniform_real_distribution<float>& distribution);
	Vec3f renderPixel(const Camera& camera, int i, int j, std::default_random_engine& randGenerator, std::uniform_real_distribution<float>& distribution);
	Ray generateRay(const Camera& camera, int i, int j, float time, float dx = 0.5f, float dy = 0.5f);
	Ray generateRayDepthOfField(const Camera& camera, int i, int j, float dx, float dy, float dofRandx, float dofRandy, float time);
	Vec3f findPixelColor(const Ray& ray, const Camera& camera, int depth, int i, int j, std::default_random_engine& randGenerator, std::uniform_real_distribution<float>& distribution);
	bool refractRay(Vec3f direction, Vec3f normal, float n1, float n2, Vec3f& wt);
	float findReflectionRatioDielectric(float cosTheta, float n1, float n2);
	float findReflectionRatioConductor(float cosTheta, float n1, float n2);
	Vec3f renderPixelMultisampling(const Camera& camera, int i, int j, std::default_random_engine& randGenerator, std::uniform_real_distribution<float>& distribution);
	std::vector<Ray> sampleRays(const Camera& camera, int i, int j, std::default_random_engine& randGenerator, std::uniform_real_distribution<float>& distribution);
	std::vector<Ray> sampleRaysDepthOfField(const Camera& camera, int i, int j, std::default_random_engine& randGenerator, std::uniform_real_distribution<float>& distribution);
	~Scene();

private:
	Vec3f diffuseShading(const Vec3f& irradiance, const Vec3f& wi, const Hit& hit, const Material& material, const Texture* texture);
	Vec3f specularShading(const Vec3f& irradiance, const Vec3f& wi, const Hit& hit, const Material& material, const Ray& ray);
	void applyTransformations(tinyxml2::XMLElement* element, std::stringstream& stream, Matrix4f& matrix);
	Vec3f getBackgroundColor(int i, int j, const Ray& ray) const;
	void applyDegamma(Material& material, const Tonemap& tonemap);
};


#endif