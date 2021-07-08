#include "Scene.h"
#include <sstream>
#include <stdexcept>
#include <iostream>

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
		Orientation cameraOrientation = ORIENTATION_RIGHTHANDED;
		auto handedness = element->Attribute("handedness");
		if (handedness && strcmp(handedness, "left") == 0)
		{
			cameraOrientation = ORIENTATION_LEFTHANDED;
		}
		camera.orientation = cameraOrientation;

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

			// Get Tonemap
			camera.tonemap = Tonemap();
			auto tonemapElement = element->FirstChildElement("Tonemap");
			if (tonemapElement)
			{
				child = tonemapElement->FirstChildElement("TMO");
				if (child)
				{
					stream << child->GetText() << std::endl;
				}
				else
				{
					stream << "Photographic" << std::endl;
				}

				child = tonemapElement->FirstChildElement("TMOOptions");
				stream << child->GetText() << std::endl;
				child = tonemapElement->FirstChildElement("Saturation");
				stream << child->GetText() << std::endl;
				child = tonemapElement->FirstChildElement("Gamma");
				const char* gammaValue = child->GetText();
				if (strcmp(gammaValue, "sRGB") == 0)
				{
					stream << "2.2" << std::endl;
				}
				else
				{
					stream << child->GetText() << std::endl;
				}

				std::string tonemapOperator;
				float key, burn, saturation, gamma;
				stream >> tonemapOperator;
				stream >> key >> burn >> saturation >> gamma;
				Tonemap tonemap = Tonemap(tonemapOperator, key, burn, saturation, gamma);
				camera.tonemap = tonemap;
			}
		}
		else if (strcmp(type, "lookAt") == 0)
		{
			// Parse lookAt camera object
			bool gazeVectorExists = false;
			auto child = element->FirstChildElement("Position");
			stream << child->GetText() << std::endl;
			child = element->FirstChildElement("GazePoint");
			if (!child)
			{
				child = element->FirstChildElement("Gaze");
				gazeVectorExists = true;
			}
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

			child = element->FirstChildElement("Renderer");
			if (child)
			{
				stream << child->GetText() << std::endl;
			}
			else
			{
				stream << "DirectLighting" << std::endl;
			}

			stream >> camera.position.x >> camera.position.y >> camera.position.z;
			Vec3f gazePoint;
			stream >> gazePoint.x >> gazePoint.y >> gazePoint.z;
			if (gazeVectorExists)
			{
				camera.gaze = gazePoint;
			}
			else
			{
				camera.gaze = gazePoint - camera.position;
			}
			stream >> camera.v.x >> camera.v.y >> camera.v.z;
			float fovy;
			stream >> fovy;
			stream >> camera.distance;
			stream >> camera.focusDistance;
			stream >> camera.apertureSize;
			stream >> camera.imageWidth >> camera.imageHeight;
			stream >> camera.imageName;
			stream >> camera.numberOfSamples;

			// Calculate image plane parameters
			camera.top = tan((fovy * PI) / (2 * 180.0f)) * camera.distance;
			camera.right = camera.top * (float(camera.imageWidth) / float(camera.imageHeight));
			camera.bottom = -1 * camera.top;
			camera.left = -1 * camera.right;

			// Set renderer
			std::string renderingMode;
			stream >> renderingMode;

			if (renderingMode == "DirectLighting")
			{
				camera.renderingMode = RENDERINGMODE_RAYTRACING;
			}
			else if (renderingMode == "PathTracing")
			{
				camera.renderingMode = RENDERINGMODE_PATHTRACING;
			}

			// Get renderer parameters
			camera.nextEventEstimation = false;
			camera.russianRoulette = false;
			camera.importanceSampling = false;

			child = element->FirstChildElement("RendererParams");
			if (child)
			{
				if (camera.renderingMode == RENDERINGMODE_PATHTRACING)
				{
					getRendererParams(child, stream, camera);
				}
			}

			// Get Tonemap
			camera.tonemap = Tonemap();
			auto tonemapElement = element->FirstChildElement("Tonemap");
			if (tonemapElement)
			{
				child = tonemapElement->FirstChildElement("TMO");
				if (child)
				{
					stream << child->GetText() << std::endl;
				}
				else
				{
					stream << "Photographic" << std::endl;
				}

				child = tonemapElement->FirstChildElement("TMOOptions");
				stream << child->GetText() << std::endl;
				child = tonemapElement->FirstChildElement("Saturation");
				stream << child->GetText() << std::endl;
				child = tonemapElement->FirstChildElement("Gamma");
				const char* gammaValue = child->GetText();
				if (strcmp(gammaValue, "sRGB") == 0)
				{
					stream << "2.2" << std::endl;
				}
				else
				{
					stream << child->GetText() << std::endl;
				}

				std::string tonemapOperator;
				float key, burn, saturation, gamma;
				stream >> tonemapOperator;
				stream >> key >> burn >> saturation >> gamma;
				Tonemap tonemap = Tonemap(tonemapOperator, key, burn, saturation, gamma);
				camera.tonemap = tonemap;
			}
		}

		cameras.push_back(camera);
		element = element->NextSiblingElement("Camera");
	}
	stream.clear();

	// Get Lights
	auto lightsElement = root->FirstChildElement("Lights");
	if (lightsElement)
	{
		// Get Ambient Light
		auto child = lightsElement->FirstChildElement("AmbientLight");
		if (child)
		{
			stream << child->GetText() << std::endl;
		}
		else
		{
			stream << "0 0 0" << std::endl;
		}
		stream >> ambientLight.x >> ambientLight.y >> ambientLight.z;

		// Get Point Lights
		element = lightsElement->FirstChildElement("PointLight");
		while (element)
		{
			child = element->FirstChildElement("Position");
			stream << child->GetText() << std::endl;
			child = element->FirstChildElement("Intensity");
			stream << child->GetText() << std::endl;

			Vec3f position, intensity;
			stream >> position.x >> position.y >> position.z;
			stream >> intensity.x >> intensity.y >> intensity.z;

			lights.push_back(new PointLight(position, intensity));
			element = element->NextSiblingElement("PointLight");
		}

		// Get Area Lights
		element = lightsElement->FirstChildElement("AreaLight");
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

			lights.push_back(new AreaLight(position, normal, extent, radiance));
			element = element->NextSiblingElement("AreaLight");
		}

		// Get Directional Lights
		element = lightsElement->FirstChildElement("DirectionalLight");
		while (element)
		{
			child = element->FirstChildElement("Direction");
			stream << child->GetText() << std::endl;
			child = element->FirstChildElement("Radiance");
			stream << child->GetText() << std::endl;

			Vec3f direction, radiance;
			stream >> direction.x >> direction.y >> direction.z;
			stream >> radiance.x >> radiance.y >> radiance.z;

			lights.push_back(new DirectionalLight(direction, radiance));
			element = element->NextSiblingElement("DirectionalLight");
		}

		// Get Spot Lights
		element = lightsElement->FirstChildElement("SpotLight");
		while (element)
		{
			child = element->FirstChildElement("Position");
			stream << child->GetText() << std::endl;
			child = element->FirstChildElement("Direction");
			stream << child->GetText() << std::endl;
			child = element->FirstChildElement("Intensity");
			stream << child->GetText() << std::endl;
			child = element->FirstChildElement("CoverageAngle");
			stream << child->GetText() << std::endl;
			child = element->FirstChildElement("FalloffAngle");
			stream << child->GetText() << std::endl;

			Vec3f position, direction, intensity;
			float coverageAngle, falloffAngle;
			stream >> position.x >> position.y >> position.z;
			stream >> direction.x >> direction.y >> direction.z;
			stream >> intensity.x >> intensity.y >> intensity.z;
			stream >> coverageAngle >> falloffAngle;

			coverageAngle = (coverageAngle * PI) / 180.0f;
			falloffAngle = (falloffAngle * PI) / 180.0f;

			lights.push_back(new SpotLight(position, direction, intensity, coverageAngle, falloffAngle));
			element = element->NextSiblingElement("SpotLight");
		}
	}

	// Get BRDFs
	element = root->FirstChildElement("BRDFs");
	if (element)
	{
		auto child = element->FirstChildElement("OriginalPhong");
		while (child)
		{
			auto exponentElement = child->FirstChildElement("Exponent");
			stream << exponentElement->GetText() << std::endl;
			float exponent;
			stream >> exponent;

			brdfs.push_back(new PhongBRDF(exponent));
			child = child->NextSiblingElement("OriginalPhong");
		}

		child = element->FirstChildElement("OriginalBlinnPhong");
		while (child)
		{
			auto exponentElement = child->FirstChildElement("Exponent");
			stream << exponentElement->GetText() << std::endl;
			float exponent;
			stream >> exponent;

			brdfs.push_back(new BlinnPhongBRDF(exponent));
			child = child->NextSiblingElement("OriginalBlinnPhong");
		}

		child = element->FirstChildElement("ModifiedPhong");
		while (child)
		{
			bool normalized = child->BoolAttribute("normalized", false);

			auto exponentElement = child->FirstChildElement("Exponent");
			stream << exponentElement->GetText() << std::endl;
			float exponent;
			stream >> exponent;

			brdfs.push_back(new ModifiedPhongBRDF(exponent, normalized));
			child = child->NextSiblingElement("ModifiedPhong");
		}

		child = element->FirstChildElement("ModifiedBlinnPhong");
		while (child)
		{
			bool normalized = child->BoolAttribute("normalized", false);

			auto exponentElement = child->FirstChildElement("Exponent");
			stream << exponentElement->GetText() << std::endl;
			float exponent;
			stream >> exponent;

			brdfs.push_back(new ModifiedBlinnPhongBRDF(exponent, normalized));
			child = child->NextSiblingElement("ModifiedBlinnPhong");
		}

		child = element->FirstChildElement("TorranceSparrow");
		while (child)
		{
			bool kdfresnel = child->BoolAttribute("kdfresnel", false);

			auto exponentElement = child->FirstChildElement("Exponent");
			stream << exponentElement->GetText() << std::endl;
			float exponent;
			stream >> exponent;

			brdfs.push_back(new TorranceSparrowBRDF(exponent, kdfresnel));
			child = child->NextSiblingElement("TorranceSparrow");
		}
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

		// Get Degamma
		material.degamma = element->BoolAttribute("degamma", false);
		
		// Get BRDF
		material.brdfId = element->IntAttribute("BRDF", 0);
		material.brdfId--;	// Convert to 0-based index

		// Get Material Properties
		auto child = element->FirstChildElement("AmbientReflectance");
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
	std::vector<std::string> textureImagePaths;
	element = root->FirstChildElement("Textures");
	if (element)
	{
		// Get Images
		int pos = filepath.find_last_of("/");
		std::string imageDir(filepath.substr(0, pos + 1));

		auto child = element->FirstChildElement("Images");
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

	// Get Spherical Directional (Environment) Lights
	lightsElement = root->FirstChildElement("Lights");
	if (lightsElement)
	{
		element = lightsElement->FirstChildElement("SphericalDirectionalLight");
		while (element)
		{
			int imageId;
			auto child = element->FirstChildElement("ImageId");
			stream << child->GetText() << std::endl;
			stream >> imageId;
			imageId--;	// Convert to 0-based index

			SphericalDirectionalLight *light = new SphericalDirectionalLight(textureImagePaths[imageId]);
			lights.push_back(light);
			sphericalDirLight = light;

			element = element->NextSiblingElement("SphericalDirectionalLight");
		}
	}

	// Get Transformations
	element = root->FirstChildElement("Transformations");
	if (element)
	{
		// Get Translations
		auto child = element->FirstChildElement("Translation");
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
		auto child = element->FirstChildElement("Material");
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
				int vertexIndices[3] = { vertexOffset + v0, vertexOffset + v1, vertexOffset + v2 };
				int textureIndices[3] = { textureOffset + v0, textureOffset + v1, textureOffset + v2 };

				Triangle *triangle = new Triangle(this, vertexIndices, textureIndices, materialId, texture, normalTexture, shadingMode,
					Matrix4f(1.0f), false, Vec3f(0.0f, 0.0f, 0.0f), false);
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
		bool resetTransform = element->BoolAttribute("resetTransform");

		int materialId;
		auto child = element->FirstChildElement("Material");
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
			if (resetTransform == true)
			{
				// Discard baseMesh's transformations.
				applyTransformations(child, stream, transformationMat);
			}
			else
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

	// Get LightMeshes
	element = root->FirstChildElement("Objects");
	element = element->FirstChildElement("LightMesh");
	while (element)
	{
		int materialId;
		auto child = element->FirstChildElement("Material");
		stream << child->GetText() << std::endl;
		stream >> materialId;
		materialId--;	// convert to 0-based index

		Texture* texture = NULL;
		Texture* normalTexture = NULL;

		Vec3f radiance;
		child = element->FirstChildElement("Radiance");
		stream << child->GetText() << std::endl;
		stream >> radiance.x >> radiance.y >> radiance.z;

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
				int vertexIndices[3] = { vertexOffset + v0, vertexOffset + v1, vertexOffset + v2 };
				int textureIndices[3] = { textureOffset + v0, textureOffset + v1, textureOffset + v2 };

				Triangle *triangle = new Triangle(this, vertexIndices, textureIndices, materialId, texture, normalTexture, SHADINGMODE_FLAT,
					Matrix4f(1.0f), false, Vec3f(0.0f, 0.0f, 0.0f), false);
				triangles.push_back(triangle);
			}
			stream.clear();
		}
		else
		{
			parsePlyFile(filepath, plyFile, triangles, SHADINGMODE_FLAT, materialId, texture, normalTexture, vertexOffset, textureOffset);
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

		LightMesh* lightMesh = new LightMesh(this, materialId, texture, normalTexture, triangles, transformationMat, transform, motionVec, motionBlur, radiance);
		lights.push_back(lightMesh);
		objects.push_back(lightMesh);
		element = element->NextSiblingElement("LightMesh");
	}

	// Get Triangles
	element = root->FirstChildElement("Objects");
	element = element->FirstChildElement("Triangle");
	while (element)
	{
		int materialId;
		auto child = element->FirstChildElement("Material");
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
		int vertexIndices[3] = { v0, v1, v2 };
		int textureIndices[3] = { v0, v1, v2 };

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

		objects.push_back(new Triangle(this, vertexIndices, textureIndices, materialId, texture, normalTexture, SHADINGMODE_FLAT,
			transformationMat, transform, motionVec, motionBlur));
		element = element->NextSiblingElement("Triangle");
	}

	// Get Spheres
	element = root->FirstChildElement("Objects");
	element = element->FirstChildElement("Sphere");
	while (element)
	{
		int materialId;
		auto child = element->FirstChildElement("Material");
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

	// Get LightSpheres
	element = root->FirstChildElement("Objects");
	element = element->FirstChildElement("LightSphere");
	while (element)
	{
		int materialId;
		auto child = element->FirstChildElement("Material");
		stream << child->GetText() << std::endl;
		stream >> materialId;
		materialId--;	// convert to 0-based index

		Texture* texture = NULL;
		Texture* normalTexture = NULL;

		Vec3f radiance;
		child = element->FirstChildElement("Radiance");
		stream << child->GetText() << std::endl;
		stream >> radiance.x >> radiance.y >> radiance.z;

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

		LightSphere* lightSphere = new LightSphere(this, centerVertexId, radius, materialId, texture, normalTexture, transformationMat, transform, motionVec, motionBlur, radiance);
		lights.push_back(lightSphere);
		objects.push_back(lightSphere);
		element = element->NextSiblingElement("LightSphere");
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
	std::vector<std::array<double, 2>> uvTexture = plyData.getTextureUv();

	int numberOfExistingVertices = vertexData.size();
	for (int i = 0; i < vertexPositions.size(); i++)
	{
		Vec3f vertex = Vec3f(vertexPositions[i][0], vertexPositions[i][1], vertexPositions[i][2]);
		vertexData.push_back(vertex);
	}

	int numberOfExistingTex = textureCoordData.size();
	for (int i = 0; i < uvTexture.size(); i++)
	{
		Vec2f uv = Vec2f(uvTexture[i][0], uvTexture[i][1]);
		textureCoordData.push_back(uv);
	}

	vertexOffset += numberOfExistingVertices;
	textureOffset += numberOfExistingTex;
	for (int i = 0; i < faceIndices.size(); i++)
	{
		if (faceIndices[i].size() == 3)
		{
			// TRIANGLE FACES
			// Calculate indices according to offset
			int v0 = faceIndices[i][0];
			int v1 = faceIndices[i][1];
			int v2 = faceIndices[i][2];
			int vertexIndices[3] = { vertexOffset + v0, vertexOffset + v1, vertexOffset + v2 };
			int textureIndices[3] = { textureOffset + v0, textureOffset + v1, textureOffset + v2 };

			Triangle *triangle = new Triangle(this, vertexIndices, textureIndices, materialId, texture, normalTexture, shadingMode,
				Matrix4f(1.0f), false, Vec3f(0.0f, 0.0f, 0.0f), false);
			triangles.push_back(triangle);

			if (shadingMode == SHADINGMODE_SMOOTH)
			{
				vertexData[vertexIndices[0]].addToVertexNormal(triangle->normal);
				vertexData[vertexIndices[1]].addToVertexNormal(triangle->normal);
				vertexData[vertexIndices[2]].addToVertexNormal(triangle->normal);
			}
		}
		else if (faceIndices[i].size() == 4)
		{
			// QUAD FACES
			// Calculate indices according to offset
			int v0 = faceIndices[i][0];
			int v1 = faceIndices[i][1];
			int v2 = faceIndices[i][2];
			int v3 = faceIndices[i][3];
			int vertexIndices1[3] = { vertexOffset + v0, vertexOffset + v1, vertexOffset + v2 };
			int vertexIndices2[3] = { vertexOffset + v0, vertexOffset + v2, vertexOffset + v3 };
			int textureIndices1[3] = { textureOffset + v0, textureOffset + v1, textureOffset + v2 };
			int textureIndices2[3] = { textureOffset + v0, textureOffset + v2, textureOffset + v3 };

			Triangle *triangle1 = new Triangle(this, vertexIndices1, textureIndices1, materialId, texture, normalTexture, shadingMode, Matrix4f(1.0f), false, Vec3f(0.0f, 0.0f, 0.0f), false);
			Triangle *triangle2 = new Triangle(this, vertexIndices2, textureIndices2, materialId, texture, normalTexture, shadingMode, Matrix4f(1.0f), false, Vec3f(0.0f, 0.0f, 0.0f), false);
			triangles.push_back(triangle1);
			triangles.push_back(triangle2);

			if (shadingMode == SHADINGMODE_SMOOTH)
			{
				vertexData[vertexIndices1[0]].addToVertexNormal(triangle1->normal);
				vertexData[vertexIndices1[1]].addToVertexNormal(triangle1->normal);
				vertexData[vertexIndices1[2]].addToVertexNormal(triangle1->normal);

				vertexData[vertexIndices2[0]].addToVertexNormal(triangle2->normal);
				vertexData[vertexIndices2[1]].addToVertexNormal(triangle2->normal);
				vertexData[vertexIndices2[2]].addToVertexNormal(triangle2->normal);
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

void Scene::getRendererParams(tinyxml2::XMLElement* element, std::stringstream& stream, Camera& camera)
{
	std::string param;

	// Check if rendererParams are specified.
	if (element->GetText())
	{
		stream << element->GetText() << std::endl;
		while (stream >> param)
		{
			if (param == "NextEventEstimation")
			{
				camera.nextEventEstimation = true;
			}
			else if (param == "RussianRoulette")
			{
				camera.russianRoulette = true;
			}
			else if (param == "ImportanceSampling")
			{
				camera.importanceSampling = true;
			}
		}
	}

	stream.clear();
}