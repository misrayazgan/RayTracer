#ifndef TRANSFORMATION_H_
#define TRANSFORMATION_H_

#include "Matrix4f.h"

class Transformation
{
public:
	const Matrix4f& getTransformationMatrix() const;

protected:
	Matrix4f transformationMatrix;
};

class Translation : public Transformation
{
public:
	Vec3f translationVec;

	Translation(const Vec3f& translationVec_);
};

class Scaling : public Transformation
{
public:
	Vec3f scalingVec;

	Scaling(const Vec3f& scalingVec_);
};

class Rotation : public Transformation
{
public:
	float angleRad;
	Vec3f axis;

	Rotation(float angleRad_, const Vec3f& axis_);
};

class Composite : public Transformation
{
public:
	Composite(float elements_[]);
};

#endif

