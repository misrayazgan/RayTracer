#ifndef OBJECT_H_
#define OBJECT_H_

#include "Vec2f.h"
#include "Ray.h"
#include "Hit.h"
#include "BoundingBox.h"
#include "Matrix4f.h"
#include "Texture.h"

const float epsilon = 0.000001f;
class Scene;

class Object
{
public:
	int materialId;
	Texture* texture;		// Texture for shading
	Texture* normalTexture;	// Texture for normal perturbation
	Matrix4f transformationMatrix;
	Matrix4f inverseTransformationMatrix;
	Matrix4f normalTransformationMatrix;
	bool transform;
	Vec3f motionVector;
	bool motionBlur;

	Object() {}
	//Object(const Scene* scene_, int id_, const Matrix4f& matrix_, bool transform_);
	Object(const Scene* scene_, int mId_, Texture* texture_, Texture* normalTexture_, 
		const Matrix4f& matrix_, bool transform_, const Vec3f& motionVector_, bool motion_);
	virtual bool intersection(const Ray& ray, Hit& hit) = 0;
	const BoundingBox& getBoundingBox() const;
	Ray transformRay(const Ray& ray) const;
	Hit transformHit(const Hit& hit, float time) const;
	~Object();

protected:
	const Scene* scene;
	BoundingBox boundingBox;
};

#endif