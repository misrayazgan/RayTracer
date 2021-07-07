#ifndef VEC2F_H_
#define VEC2F_H_

#include <iostream>

class Vec2f
{
public:
	float x, y;
	Vec2f() : x(0.0f), y(0.0f) {}
	Vec2f(float X, float Y) : x(X), y(Y) {}

	Vec2f operator+(const Vec2f& rhs) const;
	Vec2f operator-(const Vec2f& rhs) const;
	Vec2f operator*(const Vec2f& rhs) const;
	Vec2f operator/(const Vec2f& rhs) const;

	Vec2f& operator+=(const Vec2f& rhs);
	Vec2f& operator-=(const Vec2f& rhs);
	Vec2f& operator*=(const Vec2f& rhs);
	Vec2f& operator/=(const Vec2f& rhs);

	friend Vec2f operator*(float t, const Vec2f& v);
	friend Vec2f operator/(float t, const Vec2f& v);

	Vec2f operator+(float rhs) const;
	Vec2f operator-(float rhs) const;
	Vec2f operator*(float rhs) const;
	Vec2f operator/(float rhs) const;

	bool operator==(const Vec2f& rhs) const;
	bool operator!=(const Vec2f& rhs) const;
	float operator[](int i) const;
	float& operator[](int i);

	float dotProduct(const Vec2f& rhs) const;
	float length() const;
	float lengthSquared() const;
	Vec2f unitVector();

	friend std::ostream& operator<<(std::ostream& os, const Vec2f& v);
};

inline Vec2f Vec2f::operator+(const Vec2f& rhs) const
{
	return Vec2f(x + rhs.x, y + rhs.y);
}

inline Vec2f Vec2f::operator-(const Vec2f& rhs) const
{
	return Vec2f(x - rhs.x, y - rhs.y);
}

inline Vec2f Vec2f::operator*(const Vec2f& rhs) const
{
	return Vec2f(x * rhs.x, y * rhs.y);
}

inline Vec2f Vec2f::operator/(const Vec2f& rhs) const
{
	return Vec2f(x / rhs.x, y / rhs.y);
}

inline Vec2f& Vec2f::operator+=(const Vec2f& rhs)
{
	x += rhs.x;
	y += rhs.y;
	return *this;
}

inline Vec2f& Vec2f::operator-=(const Vec2f& rhs)
{
	x -= rhs.x;
	y -= rhs.y;
	return *this;
}

inline Vec2f& Vec2f::operator*=(const Vec2f& rhs)
{
	x *= rhs.x;
	y *= rhs.y;
	return *this;
}

inline Vec2f& Vec2f::operator/=(const Vec2f& rhs)
{
	x /= rhs.x;
	y /= rhs.y;
	return *this;
}

inline Vec2f operator*(float t, const Vec2f& v)
{
	return Vec2f(v.x * t, v.y * t);
}

inline Vec2f operator/(float t, const Vec2f& v)
{
	return Vec2f(v.x / t, v.y / t);
}

inline Vec2f Vec2f::operator+(float rhs) const
{
	return Vec2f(x + rhs, y + rhs);
}

inline Vec2f Vec2f::operator-(float rhs) const
{
	return Vec2f(x - rhs, y - rhs);
}

inline Vec2f Vec2f::operator*(float rhs) const
{
	return Vec2f(x * rhs, y * rhs);
}

inline Vec2f Vec2f::operator/(float rhs) const
{
	return Vec2f(x / rhs, y / rhs);
}

inline bool Vec2f::operator==(const Vec2f& rhs) const
{
	return x == rhs.x && y == rhs.y;
}

inline bool Vec2f::operator!=(const Vec2f& rhs) const
{
	return !(*this == rhs);
}

inline float Vec2f::operator[](int i) const
{
	if (i == 0) return x;
	else if (i == 1) return y;
}

inline float& Vec2f::operator[](int i)
{
	if (i == 0) return x;
	else if (i == 1) return y;
}

inline float Vec2f::dotProduct(const Vec2f& rhs) const
{
	return x * rhs.x + y * rhs.y;
}

inline float Vec2f::length() const
{
	return sqrt(x*x + y*y);
}

inline float Vec2f::lengthSquared() const
{
	return x*x + y*y;
}

inline Vec2f Vec2f::unitVector()
{
	// Normalize the vector.
	return (*this) / this->length();
}

inline std::ostream& operator<<(std::ostream& os, const Vec2f& v)
{
	os << "Vec2f(" << v.x << ", " << v.y << ")";
	return os;
}

#endif
