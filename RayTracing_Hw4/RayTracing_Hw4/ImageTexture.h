#ifndef IMAGETEXTURE_H_
#define IMAGETEXTURE_H_

#include "Texture.h"
#include <string>

enum InterpolationType
{
	INTERPOLATION_BILINEAR = 0,
	INTERPOLATION_NEAREST
};

class ImageTexture : public Texture
{
public:
	int width;
	int height;
	int n;

	ImageTexture(const std::string& imagePath_, const std::string& interpolation_, const std::string& decal_, float normalizer_, float bumpFactor_);
	Vec3f getTextureColor(const Vec2f& uv, const Vec3f& p) const;
	Vec3f getNormalMapNormal(const Vec2f& uv, const Matrix3f& tbn) const;
	Vec3f getBumpNormal(const Vec3f& p, const Vec3f& normal, const Matrix3f& tbn, const Vec2f& uv) const;
	Vec3f fetch(int i, int j) const;

private:
	std::string imagePath;
	unsigned char *textureImage;
	InterpolationType interpolationType;
	float normalizer;
	float bumpFactor;

	Vec3f getTextureNormal(const Vec2f& uv) const;
	InterpolationType parseInterpolationType(const std::string& str);
};

#endif
