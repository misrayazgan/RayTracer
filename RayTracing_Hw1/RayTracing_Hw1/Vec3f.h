#ifndef VEC3F_H_
#define VEC3F_H_

#include <iostream>

class Vec3f
{
public:
	float x, y, z;
	Vec3f() : x(0.0f), y(0.0f), z(0.0f) {}
	Vec3f(float X, float Y, float Z) : x(X), y(Y), z(Z) {}

	Vec3f operator+(const Vec3f& rhs) const;
	Vec3f operator-(const Vec3f& rhs) const;
	Vec3f operator*(const Vec3f& rhs) const;
	Vec3f operator/(const Vec3f& rhs) const;

	Vec3f& operator+=(const Vec3f& rhs);
	Vec3f& operator-=(const Vec3f& rhs);
	Vec3f& operator*=(const Vec3f& rhs);
	Vec3f& operator/=(const Vec3f& rhs);

	friend Vec3f operator*(float t, const Vec3f& v);
	friend Vec3f operator/(float t, const Vec3f& v);
	
	Vec3f operator+(float rhs) const;
	Vec3f operator-(float rhs) const;
	Vec3f operator*(float rhs) const;
	Vec3f operator/(float rhs) const;

	bool operator==(const Vec3f& rhs) const;
	bool operator!=(const Vec3f& rhs) const;

	float dotProduct(const Vec3f& rhs) const;
	Vec3f crossProduct(const Vec3f& rhs) const;
	float length() const;
	float lengthSquared() const;
	Vec3f unitVector();

	friend std::ostream& operator<<(std::ostream& os, const Vec3f& v);
};


#endif
