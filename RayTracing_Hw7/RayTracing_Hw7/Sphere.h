#ifndef SPHERE_H_
#define SPHERE_H_

#include "Vec3f.h"
#include "Matrix3f.h"
#include "Object.h"

class Sphere : public Object
{
public:
	const int centerVertexId;
	Vec3f center;
	float radius;

	Sphere(const Scene* scene_, const int center_, float radius_, int material_, Texture* texture_, Texture* normalTexture_,
		const Matrix4f& matrix_, bool transform_, const Vec3f& motionVector_, bool motion_);
	bool intersection(const Ray& ray, Hit& hit);
	Vec2f getTextureCoords(const Vec3f& point, Texture* tex) const;

private:
	Matrix3f computeTbnMatrix(const Vec3f& point, const Vec3f& normal);
};

#endif