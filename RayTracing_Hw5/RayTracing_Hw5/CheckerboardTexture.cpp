#include "CheckerboardTexture.h"

Vec3f CheckerboardTexture::getTextureColor(const Vec2f& uv, const Vec3f& p) const
{
	Vec3f color = Vec3f();

	int x = (int)((p.x + offset) * scale) % 2;
	int y = (int)((p.y + offset) * scale) % 2;
	int z = (int)((p.z + offset) * scale) % 2;

	bool isBlack = (x + y + z) % 2;
	if (isBlack)
	{
		color = black;
	}
	else
	{
		color = white;
	}

	return color;
}

Vec3f CheckerboardTexture::getNormalMapNormal(const Vec2f& uv, const Matrix3f& tbn) const
{
	return tbn.getColumn(2);
}

Vec3f CheckerboardTexture::getBumpNormal(const Vec3f& p, const Vec3f& normal, const Matrix3f& tbn, const Vec2f& uv) const
{
	return normal;
}