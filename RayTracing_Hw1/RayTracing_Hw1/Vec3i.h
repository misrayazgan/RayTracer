#ifndef VEC3I_H_
#define VEC3I_H_

#include <iostream>

class Vec3i
{
public:
	int x, y, z;
	Vec3i() : x(0), y(0), z(0) {}
	Vec3i(int X, int Y, int Z) : x(X), y(Y), z(Z) {}

	Vec3i operator+(const Vec3i& rhs) const;
	Vec3i operator-(const Vec3i& rhs) const;
	Vec3i operator*(const Vec3i& rhs) const;
	Vec3i operator/(const Vec3i& rhs) const;

	Vec3i& operator+=(const Vec3i& rhs);
	Vec3i& operator-=(const Vec3i& rhs);
	Vec3i& operator*=(const Vec3i& rhs);
	Vec3i& operator/=(const Vec3i& rhs);

	friend Vec3i operator*(float t, const Vec3i& v);
	friend Vec3i operator/(float t, const Vec3i& v);
	
	Vec3i operator+(float rhs) const;
	Vec3i operator-(float rhs) const;
	Vec3i operator*(float rhs) const;
	Vec3i operator/(float rhs) const;

	bool operator==(const Vec3i& rhs) const;
	bool operator!=(const Vec3i& rhs) const;

	int dotProduct(const Vec3i& v1, const Vec3i& v2) const;
	Vec3i crossProduct(const Vec3i& v1, const Vec3i& v2) const;
	float length() const;
	float lengthSquared() const;

	friend std::ostream& operator<<(std::ostream& os, const Vec3i& v);
};


#endif
