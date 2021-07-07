#ifndef OBJECT_H_
#define OBJECT_H_

#include "Ray.h"
#include "BoundingBox.h"
#include "Matrix4f.h"

const float epsilon = 0.000001f;
class Scene;

class Object
{
public:
	int materialId;
	Matrix4f transformationMatrix;
	Matrix4f inverseTransformationMatrix;
	Matrix4f normalTransformationMatrix;
	bool transform;
	Vec3f motionVector;
	bool motionBlur;

	Object() {}
	//Object(const Scene* scene_, int id_, const Matrix4f& matrix_, bool transform_);
	Object(const Scene* scene_, int id_, const Matrix4f& matrix_, bool transform_, const Vec3f& motionVector_, bool motion_);
	virtual bool intersection(const Ray& ray, Hit& hit) = 0;
	const BoundingBox& getBoundingBox() const;
	Ray transformRay(const Ray& ray) const;
	Hit transformHit(const Hit& hit, float time) const;

protected:
	const Scene* scene;
	BoundingBox boundingBox;
};

#endif