#ifndef MATRIX3F_H_
#define MATRIX3F_H_

#include <iostream>
#include "Vec3f.h"

class Matrix3f
{
public:
	Matrix3f();
	Matrix3f(const float val);
	Matrix3f(const Vec3f& col1, const Vec3f& col2, const Vec3f& col3);
	float* operator[](int i);
	const float* operator[](int i) const;
	Vec3f getColumn(int i) const;
	Matrix3f operator*(const Matrix3f& rhs) const;
	Matrix3f transpose() const;
	Vec3f multiplyWithVector(const Vec3f& vector) const;
	void print() const;

private:
	float elements[3][3];
};

inline float* Matrix3f::operator[](int i)
{
	return elements[i];
}

inline const float* Matrix3f::operator[](int i) const
{
	return elements[i];
}

inline Vec3f Matrix3f::getColumn(int i) const
{
	return Vec3f(elements[0][i], elements[1][i], elements[2][i]);
}


#endif
