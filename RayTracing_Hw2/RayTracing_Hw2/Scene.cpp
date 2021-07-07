#include "Scene.h"
#include "BVH.h"
#include "tinyxml2.h"
#include "happly.h"
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
	std::vector<Object*> objects;

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

	// Get BackgroundColor
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

	// Get ShadowRayEpsilon
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

	// Get IntersectionTestEpsilon
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

	// Get MaxRecursionDepth
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

	// Get Cameras
    element = root->FirstChildElement("Cameras");
    element = element->FirstChildElement("Camera");
    Camera camera;
    while (element)
    {
		auto type = element->Attribute("type");
		if (!type)
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
		}
		else if (strcmp(type, "lookAt") == 0)
		{
			auto child = element->FirstChildElement("Position");
			stream << child->GetText() << std::endl;
			child = element->FirstChildElement("GazePoint");
			stream << child->GetText() << std::endl;
			child = element->FirstChildElement("Up");
			stream << child->GetText() << std::endl;
			child = element->FirstChildElement("FovY");
			stream << child->GetText() << std::endl;
			child = element->FirstChildElement("NearDistance");
			stream << child->GetText() << std::endl;
			child = element->FirstChildElement("ImageResolution");
			stream << child->GetText() << std::endl;
			child = element->FirstChildElement("ImageName");
			stream << child->GetText() << std::endl;

			stream >> camera.position.x >> camera.position.y >> camera.position.z;
			Vec3f gazePoint;
			stream >> gazePoint.x >> gazePoint.y >> gazePoint.z;
			camera.gaze = gazePoint - camera.position;
			stream >> camera.v.x >> camera.v.y >> camera.v.z;
			float fovy;
			stream >> fovy;
			stream >> camera.distance;
			stream >> camera.imageWidth >> camera.imageHeight;
			stream >> camera.imageName;

			camera.top = tan((fovy * PI) / (2 * 180.0f)) * camera.distance;
			camera.right = camera.top * (float(camera.imageWidth) / float(camera.imageHeight));
			camera.bottom = -1 * camera.top;
			camera.left = -1 * camera.right;
		}

		cameras.push_back(camera);
		element = element->NextSiblingElement("Camera");
    }

	// Get Lights
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

	// Get Materials
    element = root->FirstChildElement("Materials");
    element = element->FirstChildElement("Material");
    Material material;
    while (element)
    {
		// Get Material Type
		auto type = element->Attribute("type");
		if (!type)
		{
			material.type = MATERIALTYPE_NULL;
		}
		else if (strcmp(type, "mirror") == 0)
		{
			material.type = MATERIALTYPE_MIRROR;
		}
		else if (strcmp(type, "conductor") == 0)
		{
			material.type = MATERIALTYPE_CONDUCTOR;
		}
		else if (strcmp(type, "dielectric") == 0)
		{
			material.type = MATERIALTYPE_DIELECTRIC;
		}

		// Get Material Properties
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
		child = element->FirstChildElement("AbsorptionCoefficient");
        if (child)
		{
			stream << child->GetText() << std::endl;
		}
		else
		{
			stream << "0 0 0" << std::endl;
		}
		child = element->FirstChildElement("RefractionIndex");
        if (child)
		{
			stream << child->GetText() << std::endl;
		}
		else
		{
			stream << "0" << std::endl;
		}
		child = element->FirstChildElement("AbsorptionIndex");
        if (child)
		{
			stream << child->GetText() << std::endl;
		}
		else
		{
			stream << "0" << std::endl;
		}

        stream >> material.ambient.x >> material.ambient.y >> material.ambient.z;
        stream >> material.diffuse.x >> material.diffuse.y >> material.diffuse.z;
        stream >> material.specular.x >> material.specular.y >> material.specular.z;
        stream >> material.mirror.x >> material.mirror.y >> material.mirror.z;
        stream >> material.phongExponent;
		stream >> material.absorption.x >> material.absorption.y >> material.absorption.z;
		stream >> material.refractionIndex;
		stream >> material.absorptionIndex;

        materials.push_back(material);
        element = element->NextSiblingElement("Material");
    }

	// Get VertexData
    element = root->FirstChildElement("VertexData");
    stream << element->GetText() << std::endl;
    Vec3f vertex;
    while (!(stream >> vertex.x).eof())
    {
        stream >> vertex.y >> vertex.z;
        vertexData.push_back(Vertex(vertex));
    }
    stream.clear();

	// TO DO: If ply mesh is given before xml mesh, calculate vertex offsets.

	// Get Meshes
    element = root->FirstChildElement("Objects");
    element = element->FirstChildElement("Mesh");
    Mesh mesh;
    while (element)
    {
		auto mode = element->Attribute("shadingMode");
		ShadingMode shadingMode;
		if (!mode)
		{
			shadingMode = SHADINGMODE_FLAT;
		}
		else if (strcmp(mode, "smooth") == 0)
		{
			shadingMode = SHADINGMODE_SMOOTH;
		}

        child = element->FirstChildElement("Material");
        stream << child->GetText() << std::endl;
        stream >> mesh.materialId;
		mesh.materialId--;	// convert to 0-based index

        child = element->FirstChildElement("Faces");
		auto plyFile = child->Attribute("plyFile");
		if (!plyFile)
		{
			stream << child->GetText() << std::endl;
			int v0, v1, v2;
			while (!(stream >> v0).eof())
			{
				stream >> v1 >> v2;
				v0--; v1--; v2--;	// convert to 0-based index
				Triangle triangle = Triangle(this, v0, v1, v2, mesh.materialId, shadingMode);
				mesh.triangles.push_back(triangle);
				objects.push_back(new Triangle(this, v0, v1, v2, mesh.materialId, shadingMode));

				if (shadingMode == SHADINGMODE_SMOOTH)
				{
					/*vertexData[v0].addToVertexNormal(triangle.normal * triangle.area);
					vertexData[v1].addToVertexNormal(triangle.normal * triangle.area);
					vertexData[v2].addToVertexNormal(triangle.normal * triangle.area);*/

					vertexData[v0].addToVertexNormal(triangle.normal);
					vertexData[v1].addToVertexNormal(triangle.normal);
					vertexData[v2].addToVertexNormal(triangle.normal);
				}
			}
			stream.clear();
		}
		else
		{
			parsePlyFile(filepath, plyFile, mesh, objects, shadingMode);
		}

        meshes.push_back(mesh);
		mesh.triangles.clear();
        element = element->NextSiblingElement("Mesh");
    }
    stream.clear();

	// Get Triangles
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

		triangles.push_back(Triangle(this, v0 - 1, v1 - 1, v2 - 1, materialId, SHADINGMODE_FLAT));
		objects.push_back(new Triangle(this, v0 - 1, v1 - 1, v2 - 1, materialId, SHADINGMODE_FLAT));
        element = element->NextSiblingElement("Triangle");
    }

	// Get Spheres
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
		objects.push_back(new Sphere(this, centerVertexId, radius, materialId));
        element = element->NextSiblingElement("Sphere");
    }

	// Normalize vertex normals
	for (int i = 0; i < vertexData.size(); i++)
	{
		vertexData[i].vertexNormal = vertexData[i].vertexNormal / vertexData[i].numberOfAdjacentTriangles;
		vertexData[i].vertexNormal = vertexData[i].vertexNormal.unitVector();
	}

	std::cout << "Scene file is parsed successfully" << std::endl;

	// Build bounding box hierarchy
	bvh = new BVH(objects, 0, objects.size(), 0);
	std::cout << "BVH is built successfully" << std::endl;
}

void Scene::parsePlyFile(const std::string& filepath, const std::string& plyFile, Mesh& mesh, std::vector<Object*>& objects, ShadingMode shadingMode)
{
	int pos = filepath.find_last_of("/");
	std::string plyDir(filepath.substr(0, pos + 1));
	std::string plyPath = plyDir + plyFile;

	happly::PLYData plyData(plyPath);

	std::vector<std::array<double, 3>> vertexPositions = plyData.getVertexPositions();
	std::vector<std::vector<int>> faceIndices = plyData.getFaceIndices<int>();

	int numberOfExistingVertices = vertexData.size();

	for (int i = 0; i < vertexPositions.size(); i++)
	{
		Vec3f vertex = Vec3f(vertexPositions[i][0], vertexPositions[i][1], vertexPositions[i][2]);
		vertexData.push_back(vertex);
	}

	for (int i = 0; i < faceIndices.size(); i++)
	{
		// Calculate indices according to offset
		int v0 = numberOfExistingVertices + faceIndices[i][0];
		int v1 = numberOfExistingVertices + faceIndices[i][1];
		int v2 = numberOfExistingVertices + faceIndices[i][2];

		Triangle triangle = Triangle(this, v0, v1, v2, mesh.materialId, shadingMode);
		mesh.triangles.push_back(triangle);
		objects.push_back(new Triangle(this, v0, v1, v2, mesh.materialId, shadingMode));

		if (shadingMode == SHADINGMODE_SMOOTH)
		{
			/*vertexData[v0].addToVertexNormal(triangle.normal * triangle.area);
			vertexData[v1].addToVertexNormal(triangle.normal * triangle.area);
			vertexData[v2].addToVertexNormal(triangle.normal * triangle.area);*/

			vertexData[v0].addToVertexNormal(triangle.normal);
			vertexData[v1].addToVertexNormal(triangle.normal);
			vertexData[v2].addToVertexNormal(triangle.normal);
		}
	}
}

void Scene::renderScene()
{
	int numberOfCameras = cameras.size();

	for (int i = 0; i < numberOfCameras; i++)
	{
		Camera currentCamera = cameras[i];
		int width = currentCamera.imageWidth;
		int height = currentCamera.imageHeight;
		int numberOfLights = pointLights.size();
		currentCamera.SetCameraParams();

		auto start = std::chrono::system_clock::now();

		unsigned char* image = new unsigned char [width * height * 4];		

		int rgbIdx = 0;
		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < width; j++)
			{
				Vec3f color = renderPixel(currentCamera, i, j);
			
				image[rgbIdx++] = clampPixel(color.x);
				image[rgbIdx++] = clampPixel(color.y);
				image[rgbIdx++] = clampPixel(color.z);
				image[rgbIdx++] = 255;
			}
		}

		auto end = std::chrono::system_clock::now();
		lodepng::encode(currentCamera.imageName.c_str(), image, width, height);
		printTimeDuration(start, end);
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
	Vec3f color = findPixelColor(ray, camera, maxRecursionDepth);

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

	return ray;
}

Vec3f Scene::findPixelColor(const Ray& ray, const Camera& camera, int depth)
{
	Vec3f color = Vec3f();

	Hit hitResult;
	hitResult.t = kInf;
	bool result = bvh->intersection(ray, hitResult);

	if (result == true)
	{
		Material material = materials[hitResult.materialId];
			
		// Do not add shading if ray is inside an object.
		if (ray.isInsideObject == false)
		{
			color += material.ambient * ambientLight;

			// For each point light, add diffuse and specular shading to the pixel color.
			for (int i = 0; i < pointLights.size(); i++)
			{
				PointLight currentLight = pointLights[i];

				/**********Shadow check**********/
				Vec3f wi = (currentLight.position - hitResult.intersectionPoint).unitVector();
				Ray shadowRay = Ray();
				shadowRay.origin = hitResult.intersectionPoint + shadowRayEpsilon * wi;
				shadowRay.direction = wi;

				Hit shadowHit;
				shadowHit.t = kInf;
				bool shadowResult = bvh->intersection(shadowRay, shadowHit);

				float tLight = (hitResult.intersectionPoint - currentLight.position).length();
				bool shadow = false;
				if (shadowResult == true && shadowHit.t < tLight - testEpsilon && shadowHit.t > 0.0f)
				{
					shadow = true;
				}

				if (shadow == false)
				{
					Vec3f diffuse = diffuseShading(currentLight, hitResult.intersectionPoint, hitResult.normal, material);			
					Vec3f specular = specularShading(currentLight, hitResult.intersectionPoint, hitResult.normal, material, ray);
					color += diffuse + specular;
				}
			}
		}

		// Specular Reflection
		if (material.type == MATERIALTYPE_MIRROR && depth > 0)
		{
			Vec3f wo = (ray.origin - hitResult.intersectionPoint).unitVector();
			Vec3f wr = (2 * (hitResult.normal * (wo.dotProduct(hitResult.normal))) - wo).unitVector();

			Ray mirrorRay = Ray();
			mirrorRay.origin = hitResult.intersectionPoint + shadowRayEpsilon * hitResult.normal;
			mirrorRay.direction = wr;

			color += material.mirror * findPixelColor(mirrorRay, camera, depth - 1);
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
			}
			else
			{
				// Exiting ray
				cosTheta = std::abs(cosTheta);
				std::swap(n1, n2);

				bool refractionResult = refractRay(ray.direction, -1 * hitResult.normal, n1, n2, wt);
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

				Vec3f reflectionColor = findPixelColor(reflectionRay, camera, depth - 1);
				
				// Transparency is (1,1,1), there is no refracted ray.
				transparency = Vec3f(1, 1, 1);
				color += transparency * reflectionColor;
			}
			else
			{
				Ray refractionRay = Ray();
				refractionRay.origin = hitResult.intersectionPoint + shadowRayEpsilon * wt;
				refractionRay.direction = wt;
				// If entering ray, refraction is inside. Else, outside.
				refractionRay.isInsideObject = entering;

				Vec3f refractionColor = findPixelColor(refractionRay, camera, depth - 1);

				Ray reflectionRay = Ray();
				reflectionRay.origin = hitResult.intersectionPoint + shadowRayEpsilon * wr;
				reflectionRay.direction = wr;
				// If entering ray, reflection is outside. Else, inside.
				reflectionRay.isInsideObject = !entering;

				Vec3f reflectionColor = findPixelColor(reflectionRay, camera, depth - 1);
				
				transparency.x = expf(-1 * material.absorption.x * hitResult.t);
				transparency.y = expf(-1 * material.absorption.y * hitResult.t);
				transparency.z = expf(-1 * material.absorption.z * hitResult.t);
				color += transparency * (fr * reflectionColor) + (refractionColor * ft);
			}
		}

		if (material.type == MATERIALTYPE_CONDUCTOR && depth > 0)
		{
			Vec3f wo = (ray.origin - hitResult.intersectionPoint).unitVector();
			Vec3f wr = (2 * (hitResult.normal * (wo.dotProduct(hitResult.normal))) - wo).unitVector();
			
			float cosTheta = -1 * ray.direction.dotProduct(hitResult.normal);
			float fr = findReflectionRatioConductor(cosTheta, material.refractionIndex, material.absorptionIndex);

			Ray reflectionRay = Ray();
			reflectionRay.origin = hitResult.intersectionPoint + shadowRayEpsilon * hitResult.normal;
			reflectionRay.direction = wr;

			Vec3f reflectionColor = findPixelColor(reflectionRay, camera, depth - 1);
			color += material.mirror * fr * reflectionColor;	
		}
	}
	else
	{
		// No hit happened
		if (maxRecursionDepth == depth)
		{
			// Primary ray, pixel's color is background color.
			color.x = backgroundColor.x;
			color.y = backgroundColor.y;
			color.z = backgroundColor.z;
		}
		else
		{
			// Not primary ray, pixel's color is (0,0,0).
			color = Vec3f(0,0,0);
		}
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

Vec3f Scene::specularShading(const PointLight& pointLight, const Vec3f& intersectionPoint, const Vec3f& normal, const Material& material, const Ray& ray)
{
	Vec3f specular;
	Vec3f irradiance = calculateIrradiance(pointLight, intersectionPoint);
	Vec3f wi = (pointLight.position - intersectionPoint).unitVector();
	Vec3f wo = (ray.origin - intersectionPoint).unitVector();
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

Scene::~Scene()
{
	delete bvh;
}