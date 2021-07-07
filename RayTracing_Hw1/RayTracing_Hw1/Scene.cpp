#include "Scene.h"
#include "tinyxml2.h"
#include "lodepng\lodepng.h"
#include "Utils.h"
#include <sstream>
#include <stdexcept>
#include <iostream>
#include <thread>

void Scene::loadSceneFromXml(const std::string& filepath)
{
	tinyxml2::XMLDocument file;
    std::stringstream stream;

    auto res = file.LoadFile(filepath.c_str());
    if (res)
    {
        throw std::runtime_error("Error: The xml file cannot be loaded.");
    }

    auto root = file.FirstChild();
    if (!root)
    {
        throw std::runtime_error("Error: Root is not found.");
    }

	//Get BackgroundColor
    auto element = root->FirstChildElement("BackgroundColor");
    if (element)
    {
        stream << element->GetText() << std::endl;
    }
    else
    {
        stream << "0 0 0" << std::endl;
    }
    stream >> backgroundColor.x >> backgroundColor.y >> backgroundColor.z;

	//Get ShadowRayEpsilon
    element = root->FirstChildElement("ShadowRayEpsilon");
    if (element)
    {
        stream << element->GetText() << std::endl;
    }
    else
    {
        stream << "0.001" << std::endl;
    }
    stream >> shadowRayEpsilon;

	//Get IntersectionTestEpsilon
    element = root->FirstChildElement("IntersectionTestEpsilon");
    if (element)
    {
        stream << element->GetText() << std::endl;
    }
    else
    {
        stream << "0.000001" << std::endl;
    }
    stream >> testEpsilon;

	//Get MaxRecursionDepth
    element = root->FirstChildElement("MaxRecursionDepth");
    if (element)
    {
        stream << element->GetText() << std::endl;
    }
    else
    {
        stream << "0" << std::endl;
    }
    stream >> maxRecursionDepth;

	//Get Cameras
    element = root->FirstChildElement("Cameras");
    element = element->FirstChildElement("Camera");
    Camera camera;
    while (element)
    {
        auto child = element->FirstChildElement("Position");
        stream << child->GetText() << std::endl;
        child = element->FirstChildElement("Gaze");
        stream << child->GetText() << std::endl;
        child = element->FirstChildElement("Up");
        stream << child->GetText() << std::endl;
        child = element->FirstChildElement("NearPlane");
        stream << child->GetText() << std::endl;
        child = element->FirstChildElement("NearDistance");
        stream << child->GetText() << std::endl;
        child = element->FirstChildElement("ImageResolution");
        stream << child->GetText() << std::endl;
        child = element->FirstChildElement("ImageName");
        stream << child->GetText() << std::endl;

        stream >> camera.position.x >> camera.position.y >> camera.position.z;
        stream >> camera.gaze.x >> camera.gaze.y >> camera.gaze.z;
        stream >> camera.v.x >> camera.v.y >> camera.v.z;
        stream >> camera.left >> camera.right >> camera.bottom >> camera.top;
        stream >> camera.distance;
        stream >> camera.imageWidth >> camera.imageHeight;
        stream >> camera.imageName;

        cameras.push_back(camera);
        element = element->NextSiblingElement("Camera");
    }

	//Get Lights
    element = root->FirstChildElement("Lights");
    auto child = element->FirstChildElement("AmbientLight");
    stream << child->GetText() << std::endl;
    stream >> ambientLight.x >> ambientLight.y >> ambientLight.z;
    element = element->FirstChildElement("PointLight");
    PointLight pointLight;
    while (element)
    {
        child = element->FirstChildElement("Position");
        stream << child->GetText() << std::endl;
        child = element->FirstChildElement("Intensity");
        stream << child->GetText() << std::endl;

        stream >> pointLight.position.x >> pointLight.position.y >> pointLight.position.z;
        stream >> pointLight.intensity.x >> pointLight.intensity.y >> pointLight.intensity.z;

        pointLights.push_back(pointLight);
        element = element->NextSiblingElement("PointLight");
    }

	//Get Materials
    element = root->FirstChildElement("Materials");
    element = element->FirstChildElement("Material");
    Material material;
    while (element)
    {
        child = element->FirstChildElement("AmbientReflectance");
		if (child)
		{
			stream << child->GetText() << std::endl;
		}
		else
		{
			stream << "0 0 0" << std::endl;
		}
        child = element->FirstChildElement("DiffuseReflectance");
        if (child)
		{
			stream << child->GetText() << std::endl;
		}
		else
		{
			stream << "0 0 0" << std::endl;
		}
        child = element->FirstChildElement("SpecularReflectance");
        if (child)
		{
			stream << child->GetText() << std::endl;
		}
		else
		{
			stream << "0 0 0" << std::endl;
		}
        child = element->FirstChildElement("MirrorReflectance");
        if (child)
		{
			stream << child->GetText() << std::endl;
		}
		else
		{
			stream << "0 0 0" << std::endl;
		}
        child = element->FirstChildElement("PhongExponent");
        if (child)
		{
			stream << child->GetText() << std::endl;
		}
		else
		{
			stream << "1" << std::endl;
		}

        stream >> material.ambient.x >> material.ambient.y >> material.ambient.z;
        stream >> material.diffuse.x >> material.diffuse.y >> material.diffuse.z;
        stream >> material.specular.x >> material.specular.y >> material.specular.z;
        stream >> material.mirror.x >> material.mirror.y >> material.mirror.z;
        stream >> material.phongExponent;

        materials.push_back(material);
        element = element->NextSiblingElement("Material");
    }

	//Get VertexData
    element = root->FirstChildElement("VertexData");
    stream << element->GetText() << std::endl;
    Vec3f vertex;
    while (!(stream >> vertex.x).eof())
    {
        stream >> vertex.y >> vertex.z;
        vertexData.push_back(vertex);
    }
    stream.clear();

	//Get Meshes
    element = root->FirstChildElement("Objects");
    element = element->FirstChildElement("Mesh");
    Mesh mesh;
    while (element)
    {
        child = element->FirstChildElement("Material");
        stream << child->GetText() << std::endl;
        stream >> mesh.materialId;
		mesh.materialId--;	// convert to 0-based index

        child = element->FirstChildElement("Faces");
        stream << child->GetText() << std::endl;
		int v0, v1, v2;
        while (!(stream >> v0).eof())
        {
            stream >> v1 >> v2;
			mesh.triangles.push_back(Triangle(this, v0 - 1, v1 - 1, v2 - 1, mesh.materialId));
        }
        stream.clear();

        meshes.push_back(mesh);
		mesh.triangles.clear();
        element = element->NextSiblingElement("Mesh");
    }
    stream.clear();

	//Get Triangles
    element = root->FirstChildElement("Objects");
    element = element->FirstChildElement("Triangle");
    while (element)
    {
		int materialId;
        child = element->FirstChildElement("Material");
        stream << child->GetText() << std::endl;
        stream >> materialId;
		materialId--;	// convert to 0-based index

		int v0, v1, v2;
        child = element->FirstChildElement("Indices");
        stream << child->GetText() << std::endl;
        stream >> v0 >> v1 >> v2;

		triangles.push_back(Triangle(this, v0 - 1, v1 - 1, v2 - 1, materialId));
        element = element->NextSiblingElement("Triangle");
    }

	//Get Spheres
    element = root->FirstChildElement("Objects");
    element = element->FirstChildElement("Sphere");
    while (element)
    {
		int materialId;
        child = element->FirstChildElement("Material");
        stream << child->GetText() << std::endl;
        stream >> materialId;
		materialId--;	// convert to 0-based index

		int centerVertexId;
        child = element->FirstChildElement("Center");
        stream << child->GetText() << std::endl;
        stream >> centerVertexId;
		centerVertexId--;	// convert to 0-based index

		float radius;
        child = element->FirstChildElement("Radius");
        stream << child->GetText() << std::endl;
        stream >> radius;

		spheres.push_back(Sphere(this, centerVertexId, radius, materialId));
        element = element->NextSiblingElement("Sphere");
    }
}

void Scene::renderScene()
{
	int numberOfThreads = 4;
	int numberOfCameras = cameras.size();

	for (int i = 0; i < numberOfCameras; i++)
	{
		Camera currentCamera = cameras[i];
		int width = currentCamera.imageWidth;
		int height = currentCamera.imageHeight;
		int numberOfLights = pointLights.size();

		auto start = std::chrono::system_clock::now();

		unsigned char* image = new unsigned char [width * height * 4];

		int stepSize = height / numberOfThreads;
		std::vector<std::thread> threads;
		for (int k = 0; k < numberOfThreads; k++)
		{
			int minHeight = k * stepSize;
			int maxHeight = minHeight + stepSize;

			threads.push_back(std::thread(&Scene::renderScenePartial, this, minHeight, maxHeight, i, image));
		}

		for (int i = 0; i < numberOfThreads; i++)
		{
			threads[i].join();
		}

		auto end = std::chrono::system_clock::now();
		lodepng::encode(currentCamera.imageName.c_str(), image, width, height);
		printTimeDuration(start, end);
	}
}

void Scene::renderScenePartial(int minHeight, int maxHeight, int cameraIdx, unsigned char* image)
{
	Camera camera = cameras[cameraIdx];	
	camera.SetCameraParams();

	int width = camera.imageWidth;
	int rgbIdx = 4 * minHeight * width;
	for (int i = minHeight; i < maxHeight; i++)
	{
		for (int j = 0; j < width; j++)
		{
			Vec3f color = renderPixel(camera, i, j);
			
			image[rgbIdx++] = clampPixel(color.x);
			image[rgbIdx++] = clampPixel(color.y);
			image[rgbIdx++] = clampPixel(color.z);
			image[rgbIdx++] = 255;
		}
	}
}

int Scene::clampPixel(float val)
{
	int clampedVal;
	if (val > 255.0f)
	{
		clampedVal = 255;
	}
	else
	{
		clampedVal = (int)val;
	}

	return clampedVal;
}

Vec3f Scene::renderPixel(const Camera& camera, int i, int j)
{
	Ray ray = generateRay(camera, i, j);
	Hit hitResult = traceRay(ray);
	Vec3f color = findPixelColor(hitResult, camera);

	return color;
}

Ray Scene::generateRay(const Camera& camera, int i, int j)
{
	// i -> index along height
	// j -> index along width
	float su = (camera.right - camera.left)*(j + 0.5f)/(float)camera.imageWidth;
	float sv = (camera.top - camera.bottom)*(i + 0.5f)/(float)camera.imageHeight;

	Vec3f s = camera.q + su*camera.u - sv*camera.v;

	Ray ray = Ray();
	ray.origin = camera.position;
	ray.direction = (s - camera.position).unitVector();
	ray.isShadowRay = false;

	return ray;
}

Hit Scene::traceRay(const Ray& ray)
{
	Hit hitResult;
	hitResult.hitHappened = false;
	hitResult.t = std::numeric_limits<float>::infinity();

	int numberOfSpheres = spheres.size();
	int numberOfTriangles = triangles.size();
	int numberOfMeshes = meshes.size();

	/********** Intersect with spheres **********/
	for (int i = 0; i < numberOfSpheres; i++)
	{
		Sphere currentSphere = spheres[i];
		Hit hit = currentSphere.intersection(ray);
		if (hit.hitHappened && hit.t < hitResult.t - testEpsilon && hit.t > 0.0f)
		{
			hitResult = hit;
		}
	}

	/********** Intersect with triangles **********/
	for (int i = 0; i < numberOfTriangles; i++)
	{
		Triangle currentTriangle = triangles[i];
		Hit hit = currentTriangle.intersection(ray);
		if (hit.hitHappened && hit.t < hitResult.t - testEpsilon && hit.t > 0.0f)
		{
			hitResult = hit;
		}
	}

	/********** Intersect with meshes **********/
	for (int i = 0; i < numberOfMeshes; i++)
	{
		Mesh currentMesh = meshes[i];
		int nTriangles = currentMesh.triangles.size();

		for (int j = 0; j < nTriangles; j++)
		{
			Triangle currentTri = currentMesh.triangles[j];
			Hit hit = currentTri.intersection(ray);
			if (hit.hitHappened && hit.t < hitResult.t - testEpsilon && hit.t > 0.0f)
			{
				hitResult = hit;
			} 
		}
	}

	return hitResult;
}

Vec3f Scene::findPixelColor(const Hit& hitResult, const Camera& camera)
{
	Vec3f color = Vec3f();

	if (hitResult.hitHappened)
	{
		Material material = materials[hitResult.materialId];

		color = material.ambient * ambientLight;

		// For each point light, add diffuse and specular shading to the pixel color.
		for (int i = 0; i < pointLights.size(); i++)
		{
			PointLight currentLight = pointLights[i];

			/**********Shadow check**********/
			Vec3f wi = (currentLight.position - hitResult.intersectionPoint).unitVector();
			Ray shadowRay = Ray();
			shadowRay.origin = hitResult.intersectionPoint + shadowRayEpsilon * wi;
			shadowRay.direction = wi;
			shadowRay.isShadowRay = true;

			float tLight = (shadowRay.origin - currentLight.position).length();

			int numberOfSpheres = spheres.size();
			int numberOfTriangles = triangles.size();
			int numberOfMeshes = meshes.size();
			bool shadow = false;

			/********** Intersect with spheres **********/
			for (int i = 0; i < numberOfSpheres; i++)
			{
				Sphere currentSphere = spheres[i];
				Hit shadowHit = currentSphere.intersection(shadowRay);
				if (shadowHit.hitHappened && shadowHit.t < tLight - testEpsilon && shadowHit.t > 0.0f)
				{
					shadow = true;
					break;
				}
			}

			if (shadow) continue;

			/********** Intersect with triangles **********/
			for (int i = 0; i < numberOfTriangles; i++)
			{
				Triangle currentTriangle = triangles[i];
				Hit shadowHit = currentTriangle.intersection(shadowRay);
				if (shadowHit.hitHappened && shadowHit.t < tLight - testEpsilon && shadowHit.t > 0.0f)
				{
					shadow = true;
					break;
				}
			}

			if (shadow) continue;

			/********** Intersect with meshes **********/
			for (int i = 0; i < numberOfMeshes; i++)
			{
				Mesh currentMesh = meshes[i];
				int nTriangles = currentMesh.triangles.size();

				for (int j = 0; j < nTriangles; j++)
				{
					Triangle currentTri = currentMesh.triangles[j];
					Hit shadowHit = currentTri.intersection(shadowRay);
					if (shadowHit.hitHappened && shadowHit.t < tLight - testEpsilon && shadowHit.t > 0.0f)
					{
						shadow = true;
						break;
					}
				}

				if (shadow) break;
			}

			if (shadow == false)
			{
				Vec3f diffuse = diffuseShading(currentLight, hitResult.intersectionPoint, hitResult.normal, material);			
				Vec3f specular = specularShading(currentLight, hitResult.intersectionPoint, hitResult.normal, material, camera);
				color += diffuse + specular;
			}
		}
	}
	else
	{
		// No hit happened, pixel's color is background color.
		color.x = backgroundColor.x;
		color.y = backgroundColor.y;
		color.z = backgroundColor.z;
	}

	return color;
}

Vec3f Scene::diffuseShading(const PointLight& pointLight, const Vec3f& intersectionPoint, const Vec3f& normal, const Material& material)
{
	Vec3f diffuse;
	Vec3f irradiance = calculateIrradiance(pointLight, intersectionPoint);
	Vec3f wi = (pointLight.position - intersectionPoint).unitVector();
	float cosTheta = std::max(0.0f, wi.dotProduct(normal));

	diffuse = material.diffuse * cosTheta * irradiance;

	return diffuse;
}

Vec3f Scene::specularShading(const PointLight& pointLight, const Vec3f& intersectionPoint, const Vec3f& normal, const Material& material, const Camera& camera)
{
	Vec3f specular;
	Vec3f irradiance = calculateIrradiance(pointLight, intersectionPoint);
	Vec3f wi = (pointLight.position - intersectionPoint).unitVector();
	Vec3f wo = (camera.position - intersectionPoint).unitVector();
	Vec3f h = (wi + wo).unitVector();
	float cosAlpha = std::max(0.0f, normal.dotProduct(h));

	specular = material.specular * powf(cosAlpha, material.phongExponent) * irradiance;

	return specular;
}

Vec3f Scene::calculateIrradiance(const PointLight& pointLight, const Vec3f& intersectionPoint)
{
	Vec3f irradiance;
	float lightDistanceSquared = (intersectionPoint - pointLight.position).lengthSquared();

	if (lightDistanceSquared != 0.0f)
	{
		irradiance = pointLight.intensity / lightDistanceSquared;
	}

	return irradiance;
}