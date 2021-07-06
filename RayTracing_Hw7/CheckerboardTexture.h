#ifndef CHECKERBOARD_H_
#define CHECKERBOARD_H_

#include "Texture.h"

class CheckerboardTexture : public Texture
{
public:
	CheckerboardTexture(const Vec3f& black_, const Vec3f& white_, float scale_, float offset_, const std::string& decalMode_)
		: Texture(decalMode_), black(black_), white(white_), scale(scale_), offset(offset_) {}
	Vec3f getTextureColor(const Vec2f& uv, const Vec3f& p) const;
	Vec3f getNormalMapNormal(const Vec2f& uv, const Matrix3f& tbn) const;
	Vec3f getBumpNormal(const Vec3f& p, const Vec3f& normal, const Matrix3f& tbn, const Vec2f& uv) const;

private:
	Vec3f black;
	Vec3f white;
	float scale;
	float offset;
};


#endif
