#include "Matrix3f.h"

Matrix3f::Matrix3f()
{
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			elements[i][j] = 0.0f;
		}
	}
}

Matrix3f::Matrix3f(const float val)
{
	// Initialize elements with val for diagonal, zero for the rest.
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			elements[i][j] = 0.0f;
		}
	}

	for (int i = 0; i < 3; i++)
	{
		elements[i][i] = 1.0f;
	}
}

Matrix3f::Matrix3f(const Vec3f& col1, const Vec3f& col2, const Vec3f& col3)
{
	for (int i = 0; i < 3; i++)
	{
		elements[i][0] = col1[i];
	}

	for (int i = 0; i < 3; i++)
	{
		elements[i][1] = col2[i];
	}

	for (int i = 0; i < 3; i++)
	{
		elements[i][2] = col3[i];
	}
}

Matrix3f Matrix3f::operator*(const Matrix3f& rhs) const
{
	Matrix3f result = Matrix3f();

	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			for (int k = 0; k < 3; k++)
			{
				result[i][j] += elements[i][k] * rhs[k][j];
			}
		}
	}

	return result;
}

Matrix3f Matrix3f::transpose() const
{
	Matrix3f result = Matrix3f();

	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			result[i][j] = elements[j][i];
		}
	}

	return result;
}

Vec3f Matrix3f::multiplyWithVector(const Vec3f& vector) const
{
	Vec3f result = Vec3f();

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

void Matrix3f::print() const
{
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			std::cout << elements[i][j] << "  ";
		}
		std::cout << std::endl;
	}
}
