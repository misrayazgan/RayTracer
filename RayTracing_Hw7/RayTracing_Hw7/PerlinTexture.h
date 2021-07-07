#ifndef PERLINTEXTURE_H_
#define PERLINTEXTURE_H_

#include "Texture.h"
#include "Vec3i.h"
#include <string>
#include <vector>
#include <chrono>
#include <random>

enum NoiseConversionType
{
	NOISECONVERSION_ABSVAL = 0,
	NOISECONVERSION_LINEAR
};

class PerlinNoise
{
public:
	PerlinNoise();
	float getNoiseValue(const Vec3f& p) const;

private:
	Vec3f G[16];
	std::vector<int> P;

	float weight(float x) const;
	float phi(int i) const;
	Vec3f getGradientVector(int i, int j, int k) const;
};

class PerlinTexture : public Texture
{
public:
	PerlinTexture(const std::string& decalMode_, const std::string& noiseType_, float scalingFactor_, float bumpFactor_);
	Vec3f getTextureColor(const Vec2f& uv, const Vec3f& p) const;
	float getPerlinNoiseValue(const Vec3f& p) const;
	Vec3f getNormalMapNormal(const Vec2f& uv, const Matrix3f& tbn) const { return Vec3f(); }
	Vec3f getBumpNormal(const Vec3f& p, const Vec3f& normal, const Matrix3f& tbn, const Vec2f& uv) const;

private:
	NoiseConversionType noiseConversionType;
	float scalingFactor;
	float bumpFactor;
	PerlinNoise perlinNoise;

	NoiseConversionType parseNoiseConversionType(const std::string& str);
};


#endif