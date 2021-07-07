#include "ImageTexture.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb-image/stb_image.h"

ImageTexture::ImageTexture(const std::string& imagePath_, const std::string& interpolation_, const std::string& decal_, float normalizer_, float bumpFactor_)
	: Texture(decal_), normalizer(normalizer_), bumpFactor(bumpFactor_), imagePath(imagePath_)
{
	interpolationType = parseInterpolationType(interpolation_);

	// Read texture image.
	textureImage = stbi_load(imagePath.c_str(), &width, &height, &n, 3);
}

Vec3f ImageTexture::getTextureColor(const Vec2f& uv, const Vec3f& p) const
{
	// Parameter p is not used in this function, added just for polymorphism.
	Vec3f color = Vec3f();

	// Find texture image coordinate at (u,v).
	float i = uv.x * width;
	float j = uv.y * height;

	if (interpolationType == INTERPOLATION_NEAREST)
	{
		// Nearest Neighbor Interpolation
		color = fetch((int)i, (int)j);
		//color = fetch((int)round(i), (int)round(j));
	}
	else
	{
		// Bilinear Interpolation
		int p = floor(i);
		int q = floor(j);
		float dx = i - p;
		float dy = j - q;

		color = fetch(p, q) * (1 - dx) * (1 - dy)
			+ fetch(p + 1, q) * dx * (1 - dy)
			+ fetch(p, q + 1) * (1 - dx) * dy
			+ fetch(p + 1, q + 1) * dx * dy;
	}

	if (decalMode != DECALMODE_REPLACEBACKGROUND)
	{
		// For background textures, return color value in range [0,255] without normalizing.
		// For others, color value is in [0,1] range.
		color = color / normalizer;
	}
	
	return color;
}

Vec3f ImageTexture::fetch(int i, int j) const
{
	Vec3f color = Vec3f();

	// Texture mode: REPEAT
	i = i % width;
	j = j % height;
	if (i < 0)
	{
		i += width;
	}
	if (j < 0)
	{
		j += height;
	}

	if (i >= width)
	{
		i = width - 1;
	}
	if (j >= height)
	{
		j = height - 1;
	}

	int rgbIdx = 3 * (j * width + i);
	color.x = textureImage[rgbIdx++];
	color.y = textureImage[rgbIdx++];
	color.z = textureImage[rgbIdx];

	return color;
}

Vec3f ImageTexture::getTextureNormal(const Vec2f& uv) const
{
	// rgb value is already normalized: in [0,1] range. Just shift it to [-0.5,0.5] range.
	Vec3f rgb = getTextureColor(uv, Vec3f());
	Vec3f normal = (rgb - Vec3f(0.5f, 0.5f, 0.5f)).unitVector();

	return normal;
}

Vec3f ImageTexture::getNormalMapNormal(const Vec2f& uv, const Matrix3f& tbn) const
{
	Vec3f textureNormal = getTextureNormal(uv);
	Vec3f newNormal = (tbn.multiplyWithVector(textureNormal)).unitVector();

	return newNormal;
}

Vec3f ImageTexture::getBumpNormal(const Vec3f& p, const Vec3f& normal, const Matrix3f& tbn, const Vec2f& uv) const
{
	Vec3f pu = tbn.getColumn(0);	// T
	Vec3f pv = tbn.getColumn(1);	// B

	// Find texture image coordinate at (u,v).
	int i = floor(uv.x * width);
	int j = floor(uv.y * height);

	float du = (fetch(i + 1, j).getAvg() - fetch(i, j).getAvg()) * bumpFactor;
	float dv = (fetch(i, j + 1).getAvg() - fetch(i, j).getAvg()) * bumpFactor;

	Vec3f qu = pu + du * normal;
	Vec3f qv = pv + dv * normal;

	Vec3f bumpedNormal = qv.crossProduct(qu).unitVector();
	if (bumpedNormal.dotProduct(normal) < 0)
	{
		bumpedNormal = -1 * bumpedNormal;
	}

	return bumpedNormal;
}

InterpolationType ImageTexture::parseInterpolationType(const std::string& str)
{
	InterpolationType type;
	if (str == "bilinear")
	{
		type = INTERPOLATION_BILINEAR;
	}
	else if (str == "nearest")
	{
		type = INTERPOLATION_NEAREST;
	}

	return type;
}
