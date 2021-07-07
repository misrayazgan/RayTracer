#include "PerlinTexture.h"

PerlinNoise::PerlinNoise()
{
	// Fill gradient vector table G.
	G[0] = Vec3f(1, 1, 0);
	G[1] = Vec3f(-1, 1, 0);
	G[2] = Vec3f(1, -1, 0);
	G[3] = Vec3f(-1, -1, 0);

	G[4] = Vec3f(1, 0, 1);
	G[5] = Vec3f(-1, 0, 1);
	G[6] = Vec3f(1, 0, -1);
	G[7] = Vec3f(-1, 0, -1);

	G[8] = Vec3f(0, 1, 1);
	G[9] = Vec3f(0, -1, 1);
	G[10] = Vec3f(0, 1, -1);
	G[11] = Vec3f(0, -1, -1);

	G[12] = Vec3f(1, 1, 0);
	G[13] = Vec3f(-1, 1, 0);
	G[14] = Vec3f(0, -1, 1);
	G[15] = Vec3f(0, -1, -1);

	// Fill P and shuffle the array.
	for (int i = 0; i < 16; i++)
	{
		P.push_back(i);
	}

	std::mt19937 randomGenerator;
	randomGenerator.seed(std::chrono::system_clock::now().time_since_epoch().count());
	std::shuffle(P.begin(), P.end(), randomGenerator);
}

float PerlinNoise::getNoiseValue(const Vec3f& p) const
{
	float noise = 0.0f;

	// Compute lattice points surrounding point p.
	int x = floor(p.x);
	int y = floor(p.y);
	int z = floor(p.z);

	std::vector<Vec3i> latticePoints;
	latticePoints.push_back(Vec3i(x, y, z));
	latticePoints.push_back(Vec3i(x, y, z + 1));
	latticePoints.push_back(Vec3i(x, y + 1, z));
	latticePoints.push_back(Vec3i(x, y + 1, z + 1));
	latticePoints.push_back(Vec3i(x + 1, y, z));
	latticePoints.push_back(Vec3i(x + 1, y, z + 1));
	latticePoints.push_back(Vec3i(x + 1, y + 1, z));
	latticePoints.push_back(Vec3i(x + 1, y + 1, z + 1));

	for (int n = 0; n < latticePoints.size(); n++)
	{
		Vec3i latticePoint = latticePoints[n];
		int i = latticePoint.x;
		int j = latticePoint.y;
		int k = latticePoint.z;

		Vec3f e = getGradientVector(i, j, k);
		Vec3f v = p - Vec3f((float)i, (float)j, (float)k);

		noise += e.dotProduct(v) * weight(v.x) * weight(v.y) * weight(v.z);
	}

	return noise;
}

float PerlinNoise::weight(float x) const
{
	x = fabs(x);
	return -6 * pow(x, 5) + 15 * pow(x, 4) - 10 * pow(x, 3) + 1;
}

float PerlinNoise::phi(int i) const
{
	i = i % 16;
	if (i < 0)
	{
		i += 16;
	}

	return P[i];
}

Vec3f PerlinNoise::getGradientVector(int i, int j, int k) const
{
	int idx = phi(i + phi(j + phi(k)));
	return G[idx];
}

PerlinTexture::PerlinTexture(const std::string& decalMode_, const std::string& noiseType_, float scalingFactor_, float bumpFactor_)
	: Texture(decalMode_), scalingFactor(scalingFactor_), bumpFactor(bumpFactor_)
{
	noiseConversionType = parseNoiseConversionType(noiseType_);
	perlinNoise = PerlinNoise();
}

Vec3f PerlinTexture::getTextureColor(const Vec2f& uv, const Vec3f& p) const
{
	float noise = getPerlinNoiseValue(p);

	return Vec3f(noise, noise, noise);
}

float PerlinTexture::getPerlinNoiseValue(const Vec3f& p) const
{
	Vec3f scaledPosition = p * scalingFactor;
	float noise = perlinNoise.getNoiseValue(scaledPosition);

	if (noiseConversionType == NOISECONVERSION_ABSVAL)
	{
		noise = abs(noise);
	}
	else if (noiseConversionType == NOISECONVERSION_LINEAR)
	{
		noise = (noise + 1.0f) / 2.0f;
	}

	return noise;
}

Vec3f PerlinTexture::getBumpNormal(const Vec3f& p, const Vec3f& normal, const Matrix3f& tbn, const Vec2f& uv) const
{
	const float epsilon = 0.001;
	float noise = getPerlinNoiseValue(p);

	// Calculate gradient at p.
	Vec3f g = Vec3f();
	g.x = ((getPerlinNoiseValue(Vec3f(p.x + epsilon, p.y, p.z)) - noise) / epsilon) * bumpFactor;
	g.y = ((getPerlinNoiseValue(Vec3f(p.x, p.y + epsilon, p.z)) - noise) / epsilon) * bumpFactor;
	g.z = ((getPerlinNoiseValue(Vec3f(p.x, p.y, p.z + epsilon)) - noise) / epsilon) * bumpFactor;

	Vec3f gTangent = (g.dotProduct(normal)) * normal;
	Vec3f gProjected = g - gTangent;
	Vec3f bumpedNormal = (normal - gProjected).unitVector();

	return bumpedNormal;
}

NoiseConversionType PerlinTexture::parseNoiseConversionType(const std::string& str)
{
	NoiseConversionType noiseType;

	if (str == "absval")
	{
		noiseType = NOISECONVERSION_ABSVAL;
	}
	else if (str == "linear")
	{
		noiseType = NOISECONVERSION_LINEAR;
	}

	return noiseType;
}