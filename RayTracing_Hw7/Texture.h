#ifndef TEXTURE_H_
#define TEXTURE_H_

#include "Vec2f.h"
#include "Vec3f.h"
#include "Matrix3f.h"

enum DecalMode
{
	DECALMODE_REPLACEKD = 0,
	DECALMODE_BLENDKD,
	DECALMODE_REPLACENORMAL,
	DECALMODE_REPLACEBACKGROUND,
	DECALMODE_REPLACEALL,
	DECALMODE_BUMPNORMAL
};

class Texture
{
public:
	bool isNormalMap;
	bool isBumpMap;

	Texture(const std::string& decalMode_);
	DecalMode getDecalMode() const;
	virtual Vec3f getTextureColor(const Vec2f& uv, const Vec3f& p) const = 0;
	virtual Vec3f getNormalMapNormal(const Vec2f& uv, const Matrix3f& tbn) const = 0;
	virtual Vec3f getBumpNormal(const Vec3f& p, const Vec3f& normal, const Matrix3f& tbn, const Vec2f& uv) const = 0;

protected:
	DecalMode decalMode;

	DecalMode parseDecalMode(const std::string& str);
};


#endif
