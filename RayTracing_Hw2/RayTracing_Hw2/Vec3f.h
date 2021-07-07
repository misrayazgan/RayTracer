#ifndef VEC3F_H_
#define VEC3F_H_

#include <iostream>
#include <algorithm>

const float kInf = std::numeric_limits<float>::infinity();

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
	float operator[](int i) const;

	float dotProduct(const Vec3f& rhs) const;
	Vec3f crossProduct(const Vec3f& rhs) const;
	float length() const;
	float lengthSquared() const;
	Vec3f unitVector();

	friend std::ostream& operator<<(std::ostream& os, const Vec3f& v);
};

inline Vec3f Vec3f::operator+(const Vec3f& rhs) const
{
	return Vec3f(x + rhs.x, y + rhs.y, z + rhs.z);
}

inline Vec3f Vec3f::operator-(const Vec3f& rhs) const
{
	return Vec3f(x - rhs.x, y - rhs.y, z - rhs.z);
}

inline Vec3f Vec3f::operator*(const Vec3f& rhs) const
{
	return Vec3f(x * rhs.x, y * rhs.y, z * rhs.z);
}

inline Vec3f Vec3f::operator/(const Vec3f& rhs) const
{
	return Vec3f(x / rhs.x, y / rhs.y, z / rhs.z);
}

inline Vec3f& Vec3f::operator+=(const Vec3f& rhs)
{
	x += rhs.x;
	y += rhs.y;
	z += rhs.z;
	return *this;
}

inline Vec3f& Vec3f::operator-=(const Vec3f& rhs)
{
	x -= rhs.x;
	y -= rhs.y;
	z -= rhs.z;
	return *this;
}

inline Vec3f& Vec3f::operator*=(const Vec3f& rhs)
{
	x *= rhs.x;
	y *= rhs.y;
	z *= rhs.z;
	return *this;
}

inline Vec3f& Vec3f::operator/=(const Vec3f& rhs)
{
	x /= rhs.x;
	y /= rhs.y;
	z /= rhs.z;
	return *this;
}

inline Vec3f operator*(float t, const Vec3f& v)
{
	return Vec3f(v.x * t, v.y * t, v.z * t);
}

inline Vec3f operator/(float t, const Vec3f& v)
{
	return Vec3f(v.x / t, v.y / t, v.z / t);
}

inline Vec3f Vec3f::operator+(float rhs) const
{
	return Vec3f(x + rhs, y + rhs, z + rhs);
}

inline Vec3f Vec3f::operator-(float rhs) const
{
	return Vec3f(x - rhs, y - rhs, z - rhs);
}

inline Vec3f Vec3f::operator*(float rhs) const
{
	return Vec3f(x * rhs, y * rhs, z * rhs);
}

inline Vec3f Vec3f::operator/(float rhs) const
{
	return Vec3f(x / rhs, y / rhs, z / rhs);
}

inline bool Vec3f::operator==(const Vec3f& rhs) const
{
	return x == rhs.x && y == rhs.y && z == rhs.z;
}

inline bool Vec3f::operator!=(const Vec3f& rhs) const
{
	return !(*this == rhs);
}

inline float Vec3f::operator[](int i) const
{
	if (i == 0) return x;
	else if (i == 1) return y;
	else if (i == 2) return z;
}

inline float Vec3f::dotProduct(const Vec3f& rhs) const
{
	return x * rhs.x + y * rhs.y + z * rhs.z;
}

inline Vec3f Vec3f::crossProduct(const Vec3f& rhs) const
{
	float vx = y * rhs.z - z * rhs.y;
	float vy = z * rhs.x - x * rhs.z;
	float vz = x * rhs.y - y * rhs.x;
	return Vec3f(vx, vy, vz);
}

inline float Vec3f::length() const
{
	return sqrt(x*x + y*y + z*z);
}

inline float Vec3f::lengthSquared() const
{
	return x*x + y*y + z*z;
}

inline Vec3f Vec3f::unitVector()
{
	// Normalize the vector.
	return (*this) / this->length();
}

inline std::ostream& operator<<(std::ostream& os, const Vec3f& v)
{
	os << "Vec3f(" << v.x << ", " << v.y << ", " << v.z << ")";
	return os;
}

#endif
