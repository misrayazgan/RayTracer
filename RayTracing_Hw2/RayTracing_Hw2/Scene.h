#ifndef SCENE_H_
#define SCENE_H_

#include "Vec3f.h"
#include "Vec3i.h"
#include "Vertex.h"
#include <vector>
#include <cmath>
#include "Camera.h"
#include "PointLight.h"
#include "Material.h"
#include "Mesh.h"
#include "Sphere.h"
#include "Ray.h"

# define PI           3.14159265358979323846

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
	std::vector<Material> materials;
    std::vector<Vertex> vertexData;
    std::vector<Mesh> meshes;
    std::vector<Triangle> triangles;
    std::vector<Sphere> spheres;

	void loadSceneFromXml(const std::string& filepath);
	void parsePlyFile(const std::string& filepath, const std::string& plyFile, Mesh& mesh, std::vector<Object*>& objects, ShadingMode shadingMode);
	void renderScene();
	Vec3f renderPixel(const Camera& camera, int i, int j);
	Ray generateRay(const Camera& camera, int i, int j);
	Vec3f findPixelColor(const Ray& ray, const Camera& camera, int depth);
	bool refractRay(Vec3f direction, Vec3f normal, float n1, float n2, Vec3f& wt);
	float findReflectionRatioDielectric(float cosTheta, float n1, float n2);
	float findReflectionRatioConductor(float cosTheta, float n1, float n2);
	~Scene();

private:
	int clampPixel(float val);
	Vec3f diffuseShading(const PointLight& pointLight, const Vec3f& intersectionPoint, const Vec3f& normal, const Material& material);
	Vec3f specularShading(const PointLight& pointLight, const Vec3f& intersectionPoint, const Vec3f& normal, const Material& material, const Ray& ray);
	Vec3f calculateIrradiance(const PointLight& pointLight, const Vec3f& intersectionPoint);
};


#endif