#include "Vec3f.h"

Vec3f Vec3f::operator+(const Vec3f& rhs) const
{
	return Vec3f(x + rhs.x, y + rhs.y, z + rhs.z);
}

Vec3f Vec3f::operator-(const Vec3f& rhs) const
{
	return Vec3f(x - rhs.x, y - rhs.y, z - rhs.z);
}

Vec3f Vec3f::operator*(const Vec3f& rhs) const
{
	return Vec3f(x * rhs.x, y * rhs.y, z * rhs.z);
}

Vec3f Vec3f::operator/(const Vec3f& rhs) const
{
	return Vec3f(x / rhs.x, y / rhs.y, z / rhs.z);
}

Vec3f& Vec3f::operator+=(const Vec3f& rhs)
{
	x += rhs.x;
	y += rhs.y;
	z += rhs.z;
	return *this;
}

Vec3f& Vec3f::operator-=(const Vec3f& rhs)
{
	x -= rhs.x;
	y -= rhs.y;
	z -= rhs.z;
	return *this;
}

Vec3f& Vec3f::operator*=(const Vec3f& rhs)
{
	x *= rhs.x;
	y *= rhs.y;
	z *= rhs.z;
	return *this;
}

Vec3f& Vec3f::operator/=(const Vec3f& rhs)
{
	x /= rhs.x;
	y /= rhs.y;
	z /= rhs.z;
	return *this;
}

Vec3f operator*(float t, const Vec3f& v)
{
	return Vec3f(v.x * t, v.y * t, v.z * t);
}

Vec3f operator/(float t, const Vec3f& v)
{
	return Vec3f(v.x / t, v.y / t, v.z / t);
}

Vec3f Vec3f::operator+(float rhs) const
{
	return Vec3f(x + rhs, y + rhs, z + rhs);
}

Vec3f Vec3f::operator-(float rhs) const
{
	return Vec3f(x - rhs, y - rhs, z - rhs);
}

Vec3f Vec3f::operator*(float rhs) const
{
	return Vec3f(x * rhs, y * rhs, z * rhs);
}

Vec3f Vec3f::operator/(float rhs) const
{
	return Vec3f(x / rhs, y / rhs, z / rhs);
}

bool Vec3f::operator==(const Vec3f& rhs) const
{
	return x == rhs.x && y == rhs.y && z == rhs.z;
}

bool Vec3f::operator!=(const Vec3f& rhs) const
{
	return !(*this == rhs);
}

float Vec3f::dotProduct(const Vec3f& rhs) const
{
	return x * rhs.x + y * rhs.y + z * rhs.z;
}

Vec3f Vec3f::crossProduct(const Vec3f& rhs) const
{
	float vx = y * rhs.z - z * rhs.y;
	float vy = z * rhs.x - x * rhs.z;
	float vz = x * rhs.y - y * rhs.x;
	return Vec3f(vx, vy, vz);
}

float Vec3f::length() const
{
	return sqrt(x*x + y*y + z*z);
}

float Vec3f::lengthSquared() const
{
	return x*x + y*y + z*z;
}

Vec3f Vec3f::unitVector()
{
	// Normalize the vector.
	return (*this) / this->length();
}

std::ostream& operator<<(std::ostream& os, const Vec3f& v)
{
	os << "Vec3f(" << v.x << ", " << v.y << ", " << v.z << ")";
	return os;
}