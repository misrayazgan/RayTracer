#ifndef TRIANGLE_H_
#define TRIANGLE_H_

#include "Vec3f.h"
#include "Matrix3f.h"
#include "Object.h"

enum ShadingMode
{
	SHADINGMODE_FLAT = 0,
	SHADINGMODE_SMOOTH
};

class Triangle : public Object
{
public:
	int v0, v1, v2;	// indices to vertexData
	int t0, t1, t2;	// indices to textureCoordData
	Vec3f normal;
	float area;
	ShadingMode shadingMode;

	Triangle(const Scene* scene_, int vertexIndices_[], int textureIndices_[], int material_, Texture* texture_, Texture* normalTexture_, ShadingMode shadingMode_,
		const Matrix4f& matrix_, bool transform_, const Vec3f& motionVector_, bool motion_);
	bool intersection(const Ray& ray, Hit& hit);
	Vec2f getTextureCoords(float beta, float gamma, Texture* tex) const;

private:
	Matrix3f tbnMatrix;

	float determinant(const Vec3f &v0, const Vec3f &v1, const Vec3f &v2);
	Matrix3f computeTbnMatrix();
};

#endif