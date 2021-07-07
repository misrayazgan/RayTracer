#ifndef SCENE_H_
#define SCENE_H_

#include "Vec3f.h"
#include "Vec3i.h"
#include <vector>
#include <cmath>
#include "Camera.h"
#include "PointLight.h"
#include "Material.h"
#include "Mesh.h"
#include "Sphere.h"
#include "Ray.h"

class Scene
{
public:
	Vec3i backgroundColor;
	float shadowRayEpsilon;
	float testEpsilon;
	int maxRecursionDepth;
	Vec3f ambientLight;

	std::vector<Camera> cameras;
	std::vector<PointLight> pointLights;
	std::vector<Material> materials;
    std::vector<Vec3f> vertexData;
    std::vector<Mesh> meshes;
    std::vector<Triangle> triangles;
    std::vector<Sphere> spheres;

	void loadSceneFromXml(const std::string& filepath);
	void renderScene();
	void renderScenePartial(int minHeight, int maxHeight, int cameraIdx, unsigned char* image);
	Vec3f renderPixel(const Camera& camera, int i, int j);
	Ray generateRay(const Camera& camera, int i, int j);
	Hit traceRay(const Ray& ray);
	Vec3f findPixelColor(const Hit& hitResult, const Camera& camera);

private:
	int clampPixel(float val);
	Vec3f diffuseShading(const PointLight& pointLight, const Vec3f& intersectionPoint, const Vec3f& normal, const Material& material);
	Vec3f specularShading(const PointLight& pointLight, const Vec3f& intersectionPoint, const Vec3f& normal, const Material& material, const Camera& camera);
	Vec3f calculateIrradiance(const PointLight& pointLight, const Vec3f& intersectionPoint);
};


#endif