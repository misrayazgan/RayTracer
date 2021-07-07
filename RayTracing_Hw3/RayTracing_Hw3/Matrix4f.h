#ifndef MATRIX4F_H_
#define MATRIX4F_H_

#include <iostream>

#include "Vec3f.h"

class Matrix4f
{
public:
	Matrix4f();
	Matrix4f(const float val);
	float* operator[](int i);
	const float* operator[](int i) const;
	Matrix4f operator*(const Matrix4f& rhs) const;
	Matrix4f operator*=(const Matrix4f& rhs);
	Matrix4f transpose() const;
	Matrix4f inverse() const;
	Vec3f multiplyWithPoint(const Vec3f& point) const;
	Vec3f multiplyWithVector(const Vec3f& vector) const;
	void print() const;

private:
	float elements[4][4];

};

inline float* Matrix4f::operator[](int i)
{
	return elements[i];
}

inline const float* Matrix4f::operator[](int i) const
{
	return elements[i];
}


#endif
