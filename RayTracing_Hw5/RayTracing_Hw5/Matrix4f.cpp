#include "Matrix4f.h"

Matrix4f::Matrix4f()
{
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			elements[i][j] = 0.0f;
		}
	}
}

Matrix4f::Matrix4f(const float val)
{
	// Initialize elements with val for diagonal, zero for the rest.
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			elements[i][j] = 0.0f;
		}
	}

	for (int i = 0; i < 4; i++)
	{
		elements[i][i] = 1.0f;
	}
}

Matrix4f Matrix4f::operator*(const Matrix4f& rhs) const
{
	Matrix4f result = Matrix4f();

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			for (int k = 0; k < 4; k++)
			{
				result[i][j] += elements[i][k] * rhs[k][j];
			}
		}
	}

	return result;
}

Matrix4f Matrix4f::operator*=(const Matrix4f& rhs)
{
	float result[4][4];

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			result[i][j] = 0;
			for (int k = 0; k < 4; k++)
			{
				result[i][j] += elements[i][k] * rhs[k][j];
			}
		}
	}

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			elements[i][j] = result[i][j];
		}
	}

	return *this;
}

Matrix4f Matrix4f::transpose() const
{
	Matrix4f result = Matrix4f();

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			result[i][j] = elements[j][i];
		}
	}

	return result;
}

// No need to check inversability.
Matrix4f Matrix4f::inverse() const
{
	Matrix4f result = Matrix4f();
	float inverse[16], m[16];
	float determinant;

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			m[i * 4 + j] = elements[i][j];
		}
	}

	inverse[0] = m[5] * m[10] * m[15] -
				 m[5] * m[11] * m[14] -
				 m[9] * m[6] * m[15] +
				 m[9] * m[7] * m[14] +
				 m[13] * m[6] * m[11] -
				 m[13] * m[7] * m[10];

	inverse[4] = -m[4] * m[10] * m[15] +
				 m[4] * m[11] * m[14] +
				 m[8] * m[6] * m[15] -
				 m[8] * m[7] * m[14] -
				 m[12] * m[6] * m[11] +
				 m[12] * m[7] * m[10];

	inverse[8] = m[4] * m[9] * m[15] -
				 m[4] * m[11] * m[13] -
				 m[8] * m[5] * m[15] +
				 m[8] * m[7] * m[13] +
				 m[12] * m[5] * m[11] -
				 m[12] * m[7] * m[9];

	inverse[12] = -m[4] * m[9] * m[14] +
				  m[4] * m[10] * m[13] +
				  m[8] * m[5] * m[14] -
				  m[8] * m[6] * m[13] -
				  m[12] * m[5] * m[10] +
				  m[12] * m[6] * m[9];

	inverse[1] = -m[1] * m[10] * m[15] +
				 m[1] * m[11] * m[14] +
				 m[9] * m[2] * m[15] -
				 m[9] * m[3] * m[14] -
				 m[13] * m[2] * m[11] +
				 m[13] * m[3] * m[10];

	inverse[5] = m[0] * m[10] * m[15] -
				 m[0] * m[11] * m[14] -
				 m[8] * m[2] * m[15] +
				 m[8] * m[3] * m[14] +
				 m[12] * m[2] * m[11] -
				 m[12] * m[3] * m[10];

	inverse[9] = -m[0] * m[9] * m[15] +
				 m[0] * m[11] * m[13] +
				 m[8] * m[1] * m[15] -
				 m[8] * m[3] * m[13] -
				 m[12] * m[1] * m[11] +
				 m[12] * m[3] * m[9];
				 
	inverse[13] = m[0] * m[9] * m[14] -
				  m[0] * m[10] * m[13] -
				  m[8] * m[1] * m[14] +
				  m[8] * m[2] * m[13] +
				  m[12] * m[1] * m[10] -
				  m[12] * m[2] * m[9];

	inverse[2] = m[1] * m[6] * m[15] -
				 m[1] * m[7] * m[14] -
				 m[5] * m[2] * m[15] +
				 m[5] * m[3] * m[14] +
				 m[13] * m[2] * m[7] -
				 m[13] * m[3] * m[6];

	inverse[6] = -m[0] * m[6] * m[15] +
				 m[0] * m[7] * m[14] +
				 m[4] * m[2] * m[15] -
				 m[4] * m[3] * m[14] -
				 m[12] * m[2] * m[7] +
				 m[12] * m[3] * m[6];

	inverse[10] = m[0] * m[5] * m[15] -
				  m[0] * m[7] * m[13] -
				  m[4] * m[1] * m[15] +
				  m[4] * m[3] * m[13] +
				  m[12] * m[1] * m[7] -
				  m[12] * m[3] * m[5];

	inverse[14] = -m[0] * m[5] * m[14] +
				  m[0] * m[6] * m[13] +
				  m[4] * m[1] * m[14] -
				  m[4] * m[2] * m[13] -
				  m[12] * m[1] * m[6] +
				  m[12] * m[2] * m[5];

	inverse[3] = -m[1] * m[6] * m[11] +
				 m[1] * m[7] * m[10] +
				 m[5] * m[2] * m[11] -
				 m[5] * m[3] * m[10] -
				 m[9] * m[2] * m[7] +
				 m[9] * m[3] * m[6];

	inverse[7] = m[0] * m[6] * m[11] -
				 m[0] * m[7] * m[10] -
				 m[4] * m[2] * m[11] +
				 m[4] * m[3] * m[10] +
				 m[8] * m[2] * m[7] -
				 m[8] * m[3] * m[6];

	inverse[11] = -m[0] * m[5] * m[11] +
				  m[0] * m[7] * m[9] +
				  m[4] * m[1] * m[11] -
				  m[4] * m[3] * m[9] -
				  m[8] * m[1] * m[7] +
				  m[8] * m[3] * m[5];

	inverse[15] = m[0] * m[5] * m[10] -
				  m[0] * m[6] * m[9] -
				  m[4] * m[1] * m[10] +
				  m[4] * m[2] * m[9] +
				  m[8] * m[1] * m[6] -
				  m[8] * m[2] * m[5];

	determinant = m[0] * inverse[0] + m[1] * inverse[4] + m[2] * inverse[8] + m[3] * inverse[12];
	determinant = 1.0f / determinant;

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			result[i][j] = inverse[i * 4 + j] * determinant;
		}
	}

	return result;
}

Vec3f Matrix4f::multiplyWithPoint(const Vec3f& point) const
{
	float p[4] = { point[0], point[1], point[2], 1.0f };
	float res[4];

	for (int i = 0; i < 4; i++)
	{
		res[i] = 0.0f;
		for (int j = 0; j < 4; j++)
		{
			res[i] += elements[i][j] * p[j];
		}
	}

	Vec3f result = Vec3f(res[0], res[1], res[2]);
	return result;
}

Vec3f Matrix4f::multiplyWithVector(const Vec3f& vector) const
{
	Vec3f result = Vec3f();

	// Simplify the multiplication, since the implicit homogeneous w coordinate is zero for a vector.
	for (int i = 0; i < 3; i++)
	{
		result[i] = 0.0f;
		for (int j = 0; j < 3; j++)
		{
			result[i] += elements[i][j] * vector[j];
		}
	}

	return result;
}

void Matrix4f::print() const
{
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			std::cout << elements[i][j] << "  ";
		}
		std::cout << std::endl;
	}
}