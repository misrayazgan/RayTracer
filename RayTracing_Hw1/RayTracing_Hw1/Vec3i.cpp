#include "Vec3i.h"

Vec3i Vec3i::operator+(const Vec3i& rhs) const
{
	return Vec3i(x + rhs.x, y + rhs.y, z + rhs.z);
}

Vec3i Vec3i::operator-(const Vec3i& rhs) const
{
	return Vec3i(x - rhs.x, y - rhs.y, z - rhs.z);
}

Vec3i Vec3i::operator*(const Vec3i& rhs) const
{
	return Vec3i(x * rhs.x, y * rhs.y, z * rhs.z);
}

Vec3i Vec3i::operator/(const Vec3i& rhs) const
{
	return Vec3i(x / rhs.x, y / rhs.y, z / rhs.z);
}

Vec3i& Vec3i::operator+=(const Vec3i& rhs)
{
	x += rhs.x;
	y += rhs.y;
	z += rhs.z;
	return *this;
}

Vec3i& Vec3i::operator-=(const Vec3i& rhs)
{
	x -= rhs.x;
	y -= rhs.y;
	z -= rhs.z;
	return *this;
}

Vec3i& Vec3i::operator*=(const Vec3i& rhs)
{
	x *= rhs.x;
	y *= rhs.y;
	z *= rhs.z;
	return *this;
}

Vec3i& Vec3i::operator/=(const Vec3i& rhs)
{
	x /= rhs.x;
	y /= rhs.y;
	z /= rhs.z;
	return *this;
}

Vec3i operator*(float t, const Vec3i& v)
{
	return Vec3i(v.x * t, v.y * t, v.z * t);
}

Vec3i operator/(float t, const Vec3i& v)
{
	return Vec3i(v.x / t, v.y / t, v.z / t);
}

Vec3i Vec3i::operator+(float rhs) const
{
	return Vec3i(x + rhs, y + rhs, z + rhs);
}

Vec3i Vec3i::operator-(float rhs) const
{
	return Vec3i(x - rhs, y - rhs, z - rhs);
}

Vec3i Vec3i::operator*(float rhs) const
{
	return Vec3i(x * rhs, y * rhs, z * rhs);
}

Vec3i Vec3i::operator/(float rhs) const
{
	return Vec3i(x / rhs, y / rhs, z / rhs);
}

bool Vec3i::operator==(const Vec3i& rhs) const
{
	return x == rhs.x && y == rhs.y && z == rhs.z;
}

bool Vec3i::operator!=(const Vec3i& rhs) const
{
	return !(*this == rhs);
}

int Vec3i::dotProduct(const Vec3i& v1, const Vec3i& v2) const
{
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

Vec3i Vec3i::crossProduct(const Vec3i& v1, const Vec3i& v2) const
{
	int vx = v1.y * v2.z - v1.z * v2.y;
	int vy = v1.z * v2.x - v1.x * v2.z;
	int vz = v1.x * v2.y - v1.y * v2.x;
	return Vec3i(vx, vy, vz);
}

float Vec3i::length() const
{
	return sqrt(x*x + y*y + z*z);
}

float Vec3i::lengthSquared() const
{
	return x*x + y*y + z*z;
}

std::ostream& operator<<(std::ostream& os, const Vec3i& v)
{
	os << "Vec3i(" << v.x << ", " << v.y << ", " << v.z << ")";
	return os;
}
