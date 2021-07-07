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

inline Vec3i Vec3i::operator+(const Vec3i& rhs) const
{
	return Vec3i(x + rhs.x, y + rhs.y, z + rhs.z);
}

inline Vec3i Vec3i::operator-(const Vec3i& rhs) const
{
	return Vec3i(x - rhs.x, y - rhs.y, z - rhs.z);
}

inline Vec3i Vec3i::operator*(const Vec3i& rhs) const
{
	return Vec3i(x * rhs.x, y * rhs.y, z * rhs.z);
}

inline Vec3i Vec3i::operator/(const Vec3i& rhs) const
{
	return Vec3i(x / rhs.x, y / rhs.y, z / rhs.z);
}

inline Vec3i& Vec3i::operator+=(const Vec3i& rhs)
{
	x += rhs.x;
	y += rhs.y;
	z += rhs.z;
	return *this;
}

inline Vec3i& Vec3i::operator-=(const Vec3i& rhs)
{
	x -= rhs.x;
	y -= rhs.y;
	z -= rhs.z;
	return *this;
}

inline Vec3i& Vec3i::operator*=(const Vec3i& rhs)
{
	x *= rhs.x;
	y *= rhs.y;
	z *= rhs.z;
	return *this;
}

inline Vec3i& Vec3i::operator/=(const Vec3i& rhs)
{
	x /= rhs.x;
	y /= rhs.y;
	z /= rhs.z;
	return *this;
}

inline Vec3i operator*(float t, const Vec3i& v)
{
	return Vec3i(v.x * t, v.y * t, v.z * t);
}

inline Vec3i operator/(float t, const Vec3i& v)
{
	return Vec3i(v.x / t, v.y / t, v.z / t);
}

inline Vec3i Vec3i::operator+(float rhs) const
{
	return Vec3i(x + rhs, y + rhs, z + rhs);
}

inline Vec3i Vec3i::operator-(float rhs) const
{
	return Vec3i(x - rhs, y - rhs, z - rhs);
}

inline Vec3i Vec3i::operator*(float rhs) const
{
	return Vec3i(x * rhs, y * rhs, z * rhs);
}

inline Vec3i Vec3i::operator/(float rhs) const
{
	return Vec3i(x / rhs, y / rhs, z / rhs);
}

inline bool Vec3i::operator==(const Vec3i& rhs) const
{
	return x == rhs.x && y == rhs.y && z == rhs.z;
}

inline bool Vec3i::operator!=(const Vec3i& rhs) const
{
	return !(*this == rhs);
}

inline int Vec3i::dotProduct(const Vec3i& v1, const Vec3i& v2) const
{
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

inline Vec3i Vec3i::crossProduct(const Vec3i& v1, const Vec3i& v2) const
{
	int vx = v1.y * v2.z - v1.z * v2.y;
	int vy = v1.z * v2.x - v1.x * v2.z;
	int vz = v1.x * v2.y - v1.y * v2.x;
	return Vec3i(vx, vy, vz);
}

inline float Vec3i::length() const
{
	return sqrt(x*x + y*y + z*z);
}

inline float Vec3i::lengthSquared() const
{
	return x*x + y*y + z*z;
}

inline std::ostream& operator<<(std::ostream& os, const Vec3i& v)
{
	os << "Vec3i(" << v.x << ", " << v.y << ", " << v.z << ")";
	return os;
}

#endif
