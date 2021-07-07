#include "Scene.h"
#include "BVH.h"
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
			// Parse simple camera object
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
			child = element->FirstChildElement("FocusDistance");
			if (child)
			{
				stream << child->GetText() << std::endl;
			}
			else
			{
				stream << "0" << std::endl;
			}
			child = element->FirstChildElement("ApertureSize");
			if (child)
			{
				stream << child->GetText() << std::endl;
			}
			else
			{
				stream << "0" << std::endl;
			}
			child = element->FirstChildElement("ImageResolution");
			stream << child->GetText() << std::endl;
			child = element->FirstChildElement("NumSamples");
			if (child)
			{
				stream << child->GetText() << std::endl;
			}
			else
			{
				stream << "1" << std::endl;
			}
			child = element->FirstChildElement("ImageName");
			stream << child->GetText() << std::endl;

			stream >> camera.position.x >> camera.position.y >> camera.position.z;
			stream >> camera.gaze.x >> camera.gaze.y >> camera.gaze.z;
			stream >> camera.v.x >> camera.v.y >> camera.v.z;
			stream >> camera.left >> camera.right >> camera.bottom >> camera.top;
			stream >> camera.distance;
			stream >> camera.focusDistance;
			stream >> camera.apertureSize;
			stream >> camera.imageWidth >> camera.imageHeight;
			stream >> camera.numberOfSamples;
			stream >> camera.imageName;
		}
		else if (strcmp(type, "lookAt") == 0)
		{
			// Parse lookAt camera object
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
			child = element->FirstChildElement("FocusDistance");
			if (child)
			{
				stream << child->GetText() << std::endl;
			}
			else
			{
				stream << "0" << std::endl;
			}
			child = element->FirstChildElement("ApertureSize");
			if (child)
			{
				stream << child->GetText() << std::endl;
			}
			else
			{
				stream << "0" << std::endl;
			}
			child = element->FirstChildElement("ImageResolution");
			stream << child->GetText() << std::endl;
			child = element->FirstChildElement("ImageName");
			stream << child->GetText() << std::endl;
			child = element->FirstChildElement("NumSamples");
			if (child)
			{
				stream << child->GetText() << std::endl;
			}
			else
			{
				stream << "1" << std::endl;
			}

			stream >> camera.position.x >> camera.position.y >> camera.position.z;
			Vec3f gazePoint;
			stream >> gazePoint.x >> gazePoint.y >> gazePoint.z;
			camera.gaze = gazePoint - camera.position;
			stream >> camera.v.x >> camera.v.y >> camera.v.z;
			float fovy;
			stream >> fovy;
			stream >> camera.distance;
			stream >> camera.focusDistance;
			stream >> camera.apertureSize;
			stream >> camera.imageWidth >> camera.imageHeight;
			stream >> camera.imageName;
			stream >> camera.numberOfSamples;

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

	// Get Area Lights
	element = root->FirstChildElement("Lights");
	element = element->FirstChildElement("AreaLight");
	while (element)
	{
		child = element->FirstChildElement("Position");
		stream << child->GetText() << std::endl;
		child = element->FirstChildElement("Normal");
		stream << child->GetText() << std::endl;
		child = element->FirstChildElement("Size");
		stream << child->GetText() << std::endl;
		child = element->FirstChildElement("Radiance");
		stream << child->GetText() << std::endl;

		Vec3f position, normal, radiance;
		float extent;
		stream >> position.x >> position.y >> position.z;
		stream >> normal.x >> normal.y >> normal.z;
		normal = normal.unitVector();
		stream >> extent;
		stream >> radiance.x >> radiance.y >> radiance.z;

		areaLights.push_back(AreaLight(position, normal, extent, radiance));
		element = element->NextSiblingElement("AreaLight");
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
		child = element->FirstChildElement("Roughness");
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
		stream >> material.roughness;

		material.diffuseExists = true;
		if (material.diffuse == Vec3f(0.0f, 0.0f, 0.0f))
		{
			material.diffuseExists = false;
		}

		material.specularExists = true;
		if (material.specular == Vec3f(0.0f, 0.0f, 0.0f))
		{
			material.specularExists = false;
		}

		material.roughnessExists = true;
		if (material.roughness == 0.0f)
		{
			material.roughnessExists = false;
		}

        materials.push_back(material);
        element = element->NextSiblingElement("Material");
    }

	// Get Textures
	element = root->FirstChildElement("Textures");
	if (element)
	{
		// Get Images
		int pos = filepath.find_last_of("/");
		std::string imageDir(filepath.substr(0, pos + 1));

		std::vector<std::string> textureImagePaths;
		child = element->FirstChildElement("Images");
		if (child)
		{
			child = child->FirstChildElement("Image");
			while (child)
			{
				std::string imageName;
				stream << child->GetText() << std::endl;
				stream >> imageName;
				std::string imagePath = imageDir + imageName;
				textureImagePaths.push_back(imagePath);
				child = child->NextSiblingElement("Image");
			}
		}

		// Get Texture Maps
		element = element->FirstChildElement("TextureMap");
		while (element)
		{
			auto type = element->Attribute("type");
			if (strcmp(type, "image") == 0)
			{
				// Image Textures
				int imageId;
				child = element->FirstChildElement("ImageId");
				stream << child->GetText() << std::endl;
				stream >> imageId;
				imageId--;	// Convert to 0-based index

				child = element->FirstChildElement("DecalMode");
				stream << child->GetText() << std::endl;

				child = element->FirstChildElement("Interpolation");
				if (child)
				{
					stream << child->GetText() << std::endl;
				}
				else
				{
					stream << "bilinear" << std::endl;
				}

				child = element->FirstChildElement("Normalizer");
				if (child)
				{
					stream << child->GetText() << std::endl;
				}
				else
				{
					stream << "255.0" << std::endl;
				}

				child = element->FirstChildElement("BumpFactor");
				if (child)
				{
					stream << child->GetText() << std::endl;
				}
				else
				{
					stream << "1.0" << std::endl;
				}

				std::string interpolationType, decalMode;
				float normalizer, bumpFactor;
				stream >> decalMode;
				stream >> interpolationType;
				stream >> normalizer;
				stream >> bumpFactor;

				ImageTexture *imageTexture = new ImageTexture(textureImagePaths[imageId], interpolationType, decalMode, normalizer, bumpFactor);
				textures.push_back(imageTexture);

				// Background texture
				if (imageTexture->getDecalMode() == DECALMODE_REPLACEBACKGROUND)
				{
					backgroundTexture = imageTexture;
				}
			}
			else if (strcmp(type, "perlin") == 0)
			{
				// Perlin Textures
				child = element->FirstChildElement("DecalMode");
				stream << child->GetText() << std::endl;
				child = element->FirstChildElement("NoiseConversion");
				stream << child->GetText() << std::endl;

				child = element->FirstChildElement("NoiseScale");
				if (child)
				{
					stream << child->GetText() << std::endl;
				}
				else
				{
					stream << "1.0" << std::endl;
				}
				
				child = element->FirstChildElement("BumpFactor");
				if (child)
				{
					stream << child->GetText() << std::endl;
				}
				else
				{
					stream << "1.0" << std::endl;
				}

				std::string decalMode, noiseConversion;
				float noiseScale, bumpFactor;
				stream >> decalMode;
				stream >> noiseConversion;
				stream >> noiseScale;
				stream >> bumpFactor;

				PerlinTexture *perlinTexture = new PerlinTexture(decalMode, noiseConversion, noiseScale, bumpFactor);
				textures.push_back(perlinTexture);
			}
			else if (strcmp(type, "checkerboard") == 0)
			{
				// Checkerboard Textures
				child = element->FirstChildElement("DecalMode");
				stream << child->GetText() << std::endl;

				child = element->FirstChildElement("BlackColor");
				if (child)
				{
					stream << child->GetText() << std::endl;
				}
				else
				{
					stream << "0 0 0" << std::endl;
				}

				child = element->FirstChildElement("WhiteColor");
				if (child)
				{
					stream << child->GetText() << std::endl;
				}
				else
				{
					stream << "255 255 255" << std::endl;
				}

				child = element->FirstChildElement("Scale");
				if (child)
				{
					stream << child->GetText() << std::endl;
				}
				else
				{
					stream << "1.0" << std::endl;
				}

				child = element->FirstChildElement("Offset");
				if (child)
				{
					stream << child->GetText() << std::endl;
				}
				else
				{
					stream << "0.01" << std::endl;
				}

				std::string decalMode;
				Vec3f black, white;
				float scale, offset;
				stream >> decalMode;
				stream >> black.x >> black.y >> black.z;
				stream >> white.x >> white.y >> white.z;
				stream >> scale >> offset;

				CheckerboardTexture *checkTexture = new CheckerboardTexture(black, white, scale, offset, decalMode);
				textures.push_back(checkTexture);
			}

			element = element->NextSiblingElement("TextureMap");
		}
	}
	stream.clear();

	// Get Transformations
	element = root->FirstChildElement("Transformations");
	if (element)
	{
		// Get Translations
		child = element->FirstChildElement("Translation");
		while (child)
		{
			Vec3f translationVec;
			stream << child->GetText() << std::endl;
			stream >> translationVec.x >> translationVec.y >> translationVec.z;
			translations.push_back(Translation(translationVec));
			child = child->NextSiblingElement("Translation");
		}

		// Get Scalings
		child = element->FirstChildElement("Scaling");
		while (child)
		{
			Vec3f scalingVec;
			stream << child->GetText() << std::endl;
			stream >> scalingVec.x >> scalingVec.y >> scalingVec.z;
			scalings.push_back(Scaling(scalingVec));
			child = child->NextSiblingElement("Scaling");
		}

		// Get Rotations
		child = element->FirstChildElement("Rotation");
		while (child)
		{
			Vec3f axis;
			float angle;
			stream << child->GetText() << std::endl;
			stream >> angle >> axis.x >> axis.y >> axis.z;
			rotations.push_back(Rotation((angle * PI) / 180.0f, axis));	// Convert degree to radians.
			child = child->NextSiblingElement("Rotation");
		}

		// Get Composites
		child = element->FirstChildElement("Composite");
		while (child)
		{
			float elements[16];
			stream << child->GetText() << std::endl;
			for (int i = 0; i < 16; i++)
			{
				stream >> elements[i];
			}
			composites.push_back(Composite(elements));
			child = child->NextSiblingElement("Composite");
		}
	}
	stream.clear();

	// Get VertexData
    element = root->FirstChildElement("VertexData");
	if (element)
	{
		stream << element->GetText() << std::endl;
		Vec3f vertex;
		while (!(stream >> vertex.x).eof())
		{
			stream >> vertex.y >> vertex.z;
			vertexData.push_back(Vertex(vertex));
		}
	}
	stream.clear();

	// Get TexCoordData
	element = root->FirstChildElement("TexCoordData");
	if (element)
	{
		stream << element->GetText() << std::endl;
		Vec2f coord;
		while (!(stream >> coord.x).eof())
		{
			stream >> coord.y;
			textureCoordData.push_back(coord);
		}
	}
	stream.clear();

	// Get Meshes
    element = root->FirstChildElement("Objects");
    element = element->FirstChildElement("Mesh");
    while (element)
    {
		auto mode = element->Attribute("shadingMode");
		ShadingMode shadingMode;
		if (!mode || (mode && strcmp(mode, "flat") == 0))
		{
			shadingMode = SHADINGMODE_FLAT;
		}
		else if (strcmp(mode, "smooth") == 0)
		{
			shadingMode = SHADINGMODE_SMOOTH;
		}

		int materialId;
        child = element->FirstChildElement("Material");
        stream << child->GetText() << std::endl;
        stream >> materialId;
		materialId--;	// convert to 0-based index

		Texture* texture = NULL;
		Texture* normalTexture = NULL;
		child = element->FirstChildElement("Textures");
		if (child)
		{
			int textureId = -1;
			int normalTextureId = -1;
			stream << child->GetText() << std::endl;
			stream >> textureId >> normalTextureId;
			textureId--;		// convert to 0-based index
			normalTextureId--;	// convert to 0-based index

			if (normalTextureId > -1)
			{
				// If object has two textures, first one is shading, second one is normal texture.
				texture = textures[textureId];
				normalTexture = textures[normalTextureId];
			}
			else
			{
				// Object has only one texture.
				if (textures[textureId]->isNormalMap || textures[textureId]->isBumpMap)
				{
					normalTexture = textures[textureId];
				}
				else
				{
					texture = textures[textureId];
				}
			}
		}
		stream.clear();
		
		std::vector<Object*> triangles;
        child = element->FirstChildElement("Faces");
		int vertexOffset = child->IntAttribute("vertexOffset", 0);
		int textureOffset = child->IntAttribute("textureOffset", 0);
		auto plyFile = child->Attribute("plyFile");
		if (!plyFile)
		{
			stream << child->GetText() << std::endl;
			int v0, v1, v2;
			while (!(stream >> v0).eof())
			{
				stream >> v1 >> v2;
				v0--; v1--; v2--;	// convert to 0-based index
				Triangle *triangle = new Triangle(this, v0, v1, v2, materialId, texture, normalTexture,	shadingMode,
					Matrix4f(1.0f), false, Vec3f(0.0f, 0.0f, 0.0f), false, vertexOffset, textureOffset);
				triangles.push_back(triangle);

				if (shadingMode == SHADINGMODE_SMOOTH)
				{
					vertexData[v0].addToVertexNormal(triangle->normal);
					vertexData[v1].addToVertexNormal(triangle->normal);
					vertexData[v2].addToVertexNormal(triangle->normal);
				}
			}
			stream.clear();
		}
		else
		{
			parsePlyFile(filepath, plyFile, triangles, shadingMode, materialId, texture, normalTexture, vertexOffset, textureOffset);
		}
		
		bool transform = false;
		Matrix4f transformationMat = Matrix4f(1.0f);
		child = element->FirstChildElement("Transformations");
		if (child)
		{
			transform = true;
			applyTransformations(child, stream, transformationMat);
		}

		bool motionBlur = false;
		Vec3f motionVec = Vec3f(0.0f, 0.0f, 0.0f);
		child = element->FirstChildElement("MotionBlur");
		if (child)
		{
			motionBlur = true;
			stream << child->GetText() << std::endl;
			stream >> motionVec.x >> motionVec.y >> motionVec.z;
		}

		Mesh* baseMesh = new Mesh(this, materialId, texture, normalTexture, triangles, transformationMat, transform, motionVec, motionBlur);
		baseMeshes.push_back(baseMesh);
		objects.push_back(baseMesh);
        element = element->NextSiblingElement("Mesh");
    }
    stream.clear();

	// Get Mesh Instances
	element = root->FirstChildElement("Objects");
	element = element->FirstChildElement("MeshInstance");
	while (element)
	{
		int baseMeshId = atoi(element->Attribute("baseMeshId"));
		baseMeshId--;	// convert to 0-based index
		Mesh *baseMesh = baseMeshes[baseMeshId];
		const char* resetTransform = element->Attribute("resetTransform");

		int materialId;
		child = element->FirstChildElement("Material");
		stream << child->GetText() << std::endl;
		stream >> materialId;
		materialId--;	// convert to 0-based index

		Texture* texture = NULL;
		Texture* normalTexture = NULL;
		child = element->FirstChildElement("Textures");
		if (child)
		{
			int textureId = -1;
			int normalTextureId = -1;
			stream << child->GetText() << std::endl;
			stream >> textureId >> normalTextureId;
			textureId--;		// convert to 0-based index
			normalTextureId--;	// convert to 0-based index

			if (normalTextureId > -1)
			{
				// If object has two textures, first one is shading, second one is normal texture.
				texture = textures[textureId];
				normalTexture = textures[normalTextureId];
			}
			else
			{
				// Object has only one texture.
				if (textures[textureId]->isNormalMap || textures[textureId]->isBumpMap)
				{
					normalTexture = textures[textureId];
				}
				else
				{
					texture = textures[textureId];
				}
			}
		}
		stream.clear();

		bool transform = false;
		Matrix4f transformationMat = Matrix4f(1.0f);
		child = element->FirstChildElement("Transformations");
		if (child)
		{
			transform = true;
			if (resetTransform && strcmp(resetTransform, "true") == 0)
			{
				// Discard baseMesh's transformations.
				applyTransformations(child, stream, transformationMat);
			}
			else if (resetTransform && strcmp(resetTransform, "false") == 0)
			{
				transformationMat = baseMesh->transformationMatrix;
				applyTransformations(child, stream, transformationMat);
			}			
		}

		bool motionBlur = false;
		Vec3f motionVec = Vec3f(0.0f, 0.0f, 0.0f);
		child = element->FirstChildElement("MotionBlur");
		if (child)
		{
			motionBlur = true;
			stream << child->GetText() << std::endl;
			stream >> motionVec.x >> motionVec.y >> motionVec.z;
		}

		objects.push_back(new MeshInstance(this, materialId, texture, normalTexture, baseMesh->bvh, transformationMat, transform, motionVec, motionBlur));
		element = element->NextSiblingElement("MeshInstance");
	}

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

		Texture* texture = NULL;
		Texture* normalTexture = NULL;
		child = element->FirstChildElement("Textures");
		if (child)
		{
			int textureId = -1;
			int normalTextureId = -1;
			stream << child->GetText() << std::endl;
			stream >> textureId >> normalTextureId;
			textureId--;		// convert to 0-based index
			normalTextureId--;	// convert to 0-based index

			if (normalTextureId > -1)
			{
				// If object has two textures, first one is shading, second one is normal texture.
				texture = textures[textureId];
				normalTexture = textures[normalTextureId];
			}
			else
			{
				// Object has only one texture.
				if (textures[textureId]->isNormalMap || textures[textureId]->isBumpMap)
				{
					normalTexture = textures[textureId];
				}
				else
				{
					texture = textures[textureId];
				}
			}
		}
		stream.clear();

		int v0, v1, v2;
        child = element->FirstChildElement("Indices");
        stream << child->GetText() << std::endl;
        stream >> v0 >> v1 >> v2;
		v0--; v1--; v2--;	// convert to 0-based index

		bool transform = false;
		Matrix4f transformationMat = Matrix4f(1.0f);
		child = element->FirstChildElement("Transformations");
		if (child)
		{
			transform = true;
			applyTransformations(child, stream, transformationMat);
		}

		bool motionBlur = false;
		Vec3f motionVec = Vec3f(0.0f, 0.0f, 0.0f);
		child = element->FirstChildElement("MotionBlur");
		if (child)
		{
			motionBlur = true;
			stream << child->GetText() << std::endl;
			stream >> motionVec.x >> motionVec.y >> motionVec.z;
		}

		objects.push_back(new Triangle(this, v0, v1, v2, materialId, texture, normalTexture, SHADINGMODE_FLAT, 
			transformationMat, transform, motionVec, motionBlur));
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

		Texture* texture = NULL;
		Texture* normalTexture = NULL;
		child = element->FirstChildElement("Textures");
		if (child)
		{
			int textureId = -1;
			int normalTextureId = -1;
			stream << child->GetText() << std::endl;
			stream >> textureId >> normalTextureId;
			textureId--;		// convert to 0-based index
			normalTextureId--;	// convert to 0-based index

			if (normalTextureId > -1)
			{
				// If object has two textures, first one is shading, second one is normal texture.
				texture = textures[textureId];
				normalTexture = textures[normalTextureId];
			}
			else
			{
				// Object has only one texture.
				if (textures[textureId]->isNormalMap || textures[textureId]->isBumpMap)
				{
					normalTexture = textures[textureId];
				}
				else
				{
					texture = textures[textureId];
				}
			}
		}
		stream.clear();

		int centerVertexId;
        child = element->FirstChildElement("Center");
        stream << child->GetText() << std::endl;
        stream >> centerVertexId;
		centerVertexId--;	// convert to 0-based index

		float radius;
        child = element->FirstChildElement("Radius");
        stream << child->GetText() << std::endl;
        stream >> radius;

		bool transform = false;
		Matrix4f transformationMat = Matrix4f(1.0f);
		child = element->FirstChildElement("Transformations");
		if (child)
		{
			transform = true;
			applyTransformations(child, stream, transformationMat);
		}

		bool motionBlur = false;
		Vec3f motionVec = Vec3f(0.0f, 0.0f, 0.0f);
		child = element->FirstChildElement("MotionBlur");
		if (child)
		{
			motionBlur = true;
			stream << child->GetText() << std::endl;
			stream >> motionVec.x >> motionVec.y >> motionVec.z;
		}

		Sphere *sphere = new Sphere(this, centerVertexId, radius, materialId, texture, normalTexture, transformationMat, transform, motionVec, motionBlur);
		objects.push_back(sphere);
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

void Scene::parsePlyFile(const std::string& filepath, const std::string& plyFile, std::vector<Object*>& triangles, 
	ShadingMode shadingMode, int materialId, Texture* texture, Texture* normalTexture, int vertexOffset, int textureOffset)
{
	int pos = filepath.find_last_of("/");
	std::string plyDir(filepath.substr(0, pos + 1));
	std::string plyPath = plyDir + plyFile;

	happly::PLYData plyData(plyPath);

	std::vector<std::array<double, 3>> vertexPositions = plyData.getVertexPositions();
	std::vector<std::vector<int>> faceIndices = plyData.getFaceIndices<int>();
	//std::vector<std::array<double, 2>> uvTexture = plyData.getVertexUV();

	int numberOfExistingVertices = vertexData.size();
	vertexOffset += numberOfExistingVertices;
	for (int i = 0; i < vertexPositions.size(); i++)
	{
		Vec3f vertex = Vec3f(vertexPositions[i][0], vertexPositions[i][1], vertexPositions[i][2]);
		vertexData.push_back(vertex);
	}

	/*int numberOfExistingTex = textureCoordData.size();
	textureOffset += numberOfExistingTex;
	for (int i = 0; i < uvTexture.size(); i++)
	{
		Vec2f uv = Vec2f(uvTexture[i][0], uvTexture[i][1]);
		textureCoordData.push_back(uv);
	}*/

	for (int i = 0; i < faceIndices.size(); i++)
	{
		if (faceIndices[i].size() == 3)
		{
			// TRIANGLE FACES
			// Calculate indices according to offset
			int v0 = numberOfExistingVertices + faceIndices[i][0];
			int v1 = numberOfExistingVertices + faceIndices[i][1];
			int v2 = numberOfExistingVertices + faceIndices[i][2];

			Triangle *triangle = new Triangle(this, v0, v1, v2, materialId, texture, normalTexture, shadingMode, 
				Matrix4f(1.0f), false, Vec3f(0.0f, 0.0f, 0.0f), false, textureOffset);
			triangles.push_back(triangle);

			if (shadingMode == SHADINGMODE_SMOOTH)
			{
				vertexData[v0].addToVertexNormal(triangle->normal);
				vertexData[v1].addToVertexNormal(triangle->normal);
				vertexData[v2].addToVertexNormal(triangle->normal);
			}
		}
		else if (faceIndices[i].size() == 4)
		{
			// QUAD FACES
			// Calculate indices according to offset
			int v0 = numberOfExistingVertices + faceIndices[i][0];
			int v1 = numberOfExistingVertices + faceIndices[i][1];
			int v2 = numberOfExistingVertices + faceIndices[i][2];
			int v3 = numberOfExistingVertices + faceIndices[i][3];

			Triangle *triangle1 = new Triangle(this, v0, v1, v2, materialId, texture, normalTexture, shadingMode, Matrix4f(1.0f), false, Vec3f(0.0f, 0.0f, 0.0f), false, textureOffset);
			Triangle *triangle2 = new Triangle(this, v0, v2, v3, materialId, texture, normalTexture, shadingMode, Matrix4f(1.0f), false, Vec3f(0.0f, 0.0f, 0.0f), false, textureOffset);
			triangles.push_back(triangle1);
			triangles.push_back(triangle2);

			if (shadingMode == SHADINGMODE_SMOOTH)
			{
				vertexData[v0].addToVertexNormal(triangle1->normal);
				vertexData[v1].addToVertexNormal(triangle1->normal);
				vertexData[v2].addToVertexNormal(triangle1->normal);

				vertexData[v0].addToVertexNormal(triangle2->normal);
				vertexData[v2].addToVertexNormal(triangle2->normal);
				vertexData[v3].addToVertexNormal(triangle2->normal);
			}
		}		
	}
}

void Scene::applyTransformations(tinyxml2::XMLElement* element, std::stringstream& stream, Matrix4f& matrix)
{
	char type;
	int id;

	stream << element->GetText() << std::endl;
	while (stream >> type)
	{
		stream >> id;
		id--;	// Convert to 0-based index

		switch (type)
		{
		case 't':
			matrix = translations[id].getTransformationMatrix() * matrix;
			break;
		case 's':
			matrix = scalings[id].getTransformationMatrix() * matrix;
			break;
		case 'r':
			matrix = rotations[id].getTransformationMatrix() * matrix;
			break;
		case 'c':
			matrix = composites[id].getTransformationMatrix() * matrix;
			break;
		default:
			break;
		}
	}

	stream.clear();
}

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
		int numberOfLights = pointLights.size();

		auto start = std::chrono::system_clock::now();

		unsigned char* image = new unsigned char[width * height * 4];

		int stepSize = height / numberOfThreads;
		std::vector<std::thread> threads;
		for (int k = 0; k < numberOfThreads; k++)
		{
			int minHeight = k * stepSize;
			int maxHeight = minHeight + stepSize;

			threads.push_back(std::thread(&Scene::renderScenePartial, this, minHeight, maxHeight, i, image, std::ref(randGenerator), std::ref(distribution)));
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

void Scene::renderScenePartial(int minHeight, int maxHeight, int cameraIdx, unsigned char* image, std::default_random_engine& randGenerator, std::uniform_real_distribution<float>& distribution)
{
	Camera camera = cameras[cameraIdx];
	camera.setCameraParams();

	int width = camera.imageWidth;
	int rgbIdx = 4 * minHeight * width;

	if (camera.numberOfSamples == 1)
	{
		for (int i = minHeight; i < maxHeight; i++)
		{
			for (int j = 0; j < width; j++)
			{
				Vec3f color = renderPixel(camera, j, i, randGenerator, distribution);

				image[rgbIdx++] = clampPixel(color.x);
				image[rgbIdx++] = clampPixel(color.y);
				image[rgbIdx++] = clampPixel(color.z);
				image[rgbIdx++] = 255;
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

				image[rgbIdx++] = clampPixel(color.x);
				image[rgbIdx++] = clampPixel(color.y);
				image[rgbIdx++] = clampPixel(color.z);
				image[rgbIdx++] = 255;
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
				shadowRay.origin = hitResult.intersectionPoint + shadowRayEpsilon * hitResult.normal;
				shadowRay.direction = wi;
				shadowRay.time = ray.time;

				Hit shadowHit;
				shadowHit.t = kInf;
				bool shadowResult = bvh->intersection(shadowRay, shadowHit);

				float tLight = (currentLight.position - hitResult.intersectionPoint).length();
				bool shadow = false;
				if (shadowResult == true && shadowHit.t < tLight - testEpsilon && shadowHit.t > 0.0f)
				{
					shadow = true;
				}

				if (shadow == false)
				{
					Vec3f diffuse = diffuseShading(currentLight, hitResult, material, texture);			
					Vec3f specular = specularShading(currentLight, hitResult, material, ray);
					color += diffuse + specular;
				}
			}

			// For each area light, add diffuse and specular shading to the pixel color.
			for (int i = 0; i < areaLights.size(); i++)
			{
				AreaLight areaLight = areaLights[i];

				float e1 = distribution(randGenerator) - 0.5f;
				float e2 = distribution(randGenerator) - 0.5f;
				Vec3f pointOnLight = areaLight.position + areaLight.extent * (e1 * areaLight.u + e2 * areaLight.v);

				/**********Shadow check**********/
				Vec3f wi = (pointOnLight - hitResult.intersectionPoint).unitVector();
				Ray shadowRay = Ray();
				shadowRay.origin = hitResult.intersectionPoint + shadowRayEpsilon * hitResult.normal;
				shadowRay.direction = wi;
				shadowRay.time = ray.time;

				Hit shadowHit;
				shadowHit.t = kInf;
				bool shadowResult = bvh->intersection(shadowRay, shadowHit);

				float tLight = (pointOnLight - hitResult.intersectionPoint).length();
				bool shadow = false;
				if (shadowResult == true && shadowHit.t < tLight - testEpsilon && shadowHit.t > 0.0f)
				{
					shadow = true;
				}

				if (shadow == false)
				{
					Vec3f diffuse = diffuseShadingArea(areaLight, pointOnLight, hitResult, material, texture);
					Vec3f specular = specularShadingArea(areaLight, pointOnLight, hitResult, material, ray);
					color += diffuse + specular;
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
			color = getBackgroundColor(i, j);
		}
		// If ray is not primary ray, pixel's color is (0,0,0).
	}

	return color;
}

Vec3f Scene::getBackgroundColor(int i, int j) const
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
	else
	{
		color = backgroundColor;
	}

	return color;
}

Vec3f Scene::diffuseShading(const PointLight& pointLight, const Hit& hit, const Material& material, const Texture* texture)
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

	Vec3f irradiance = calculateIrradiance(pointLight, hit.intersectionPoint);
	Vec3f wi = (pointLight.position - hit.intersectionPoint).unitVector();
	float cosTheta = std::max(0.0f, wi.dotProduct(hit.normal));
	diffuse = kd * cosTheta * irradiance;

	return diffuse;
}

Vec3f Scene::diffuseShadingArea(const AreaLight& areaLight, const Vec3f& point, const Hit& hit, const Material& material, const Texture* texture)
{
	Vec3f diffuse = Vec3f();
	Vec3f kd = material.diffuse;

	// Handle texture
	if (texture)
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

	Vec3f wi = (point - hit.intersectionPoint).unitVector();
	Vec3f l = -1 * wi;

	// Compute irradiance
	float area = areaLight.extent * areaLight.extent;
	float lightDistanceSquared = (point - hit.intersectionPoint).lengthSquared();
	float cosThetaArea = abs(l.dotProduct(areaLight.normal));
	float dwi = (area * cosThetaArea) / lightDistanceSquared;
	Vec3f irradiance = areaLight.radiance * dwi;

	float cosTheta = std::max(0.0f, wi.dotProduct(hit.normal));
	diffuse = material.diffuse * cosTheta * irradiance;

	return diffuse;
}

Vec3f Scene::specularShading(const PointLight& pointLight, const Hit& hit, const Material& material, const Ray& ray)
{
	Vec3f specular = Vec3f();
	if (material.specularExists == true)
	{
		Vec3f irradiance = calculateIrradiance(pointLight, hit.intersectionPoint);
		Vec3f wi = (pointLight.position - hit.intersectionPoint).unitVector();
		Vec3f wo = (ray.origin - hit.intersectionPoint).unitVector();
		Vec3f h = (wi + wo).unitVector();
		float cosAlpha = std::max(0.0f, hit.normal.dotProduct(h));

		specular = material.specular * powf(cosAlpha, material.phongExponent) * irradiance;
	}	

	return specular;
}

Vec3f Scene::specularShadingArea(const AreaLight& areaLight, const Vec3f& point, const Hit& hit, const Material& material, const Ray& ray)
{
	Vec3f specular = Vec3f();
	if (material.specularExists == true)
	{
		Vec3f wi = (point - hit.intersectionPoint).unitVector();
		Vec3f wo = (ray.origin - hit.intersectionPoint).unitVector();
		Vec3f l = -1 * wi;
		Vec3f h = (wi + wo).unitVector();
		float cosAlpha = std::max(0.0f, hit.normal.dotProduct(h));

		// Compute irradiance
		float area = areaLight.extent * areaLight.extent;
		float lightDistanceSquared = (point - hit.intersectionPoint).lengthSquared();
		float cosThetaArea = abs(l.dotProduct(areaLight.normal));
		float dwi = (area * cosThetaArea) / lightDistanceSquared;
		Vec3f irradiance = areaLight.radiance * dwi;

		specular = material.specular * powf(cosAlpha, material.phongExponent) * irradiance;
	}	

	return specular;
}

Vec3f Scene::calculateIrradiance(const PointLight& pointLight, const Vec3f& intersectionPoint)
{
	Vec3f irradiance = Vec3f();
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
	if (bvh)
	{
		delete bvh;
	}

	if (backgroundTexture)
	{
		delete backgroundTexture;
	}
}