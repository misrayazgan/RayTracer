#include "Transformation.h"

const Matrix4f& Transformation::getTransformationMatrix() const
{
	return transformationMatrix;
}

Translation::Translation(const Vec3f& translationVec_)
	: translationVec(translationVec_)
{
	// Calculate transformation matrix
	transformationMatrix = Matrix4f(1.0f);
	transformationMatrix[0][3] = translationVec.x;
	transformationMatrix[1][3] = translationVec.y;
	transformationMatrix[2][3] = translationVec.z;
}

Scaling::Scaling(const Vec3f& scalingVec_)
	: scalingVec(scalingVec_)
{
	// Calculate transformation matrix
	transformationMatrix[0][0] = scalingVec.x;
	transformationMatrix[1][1] = scalingVec.y;
	transformationMatrix[2][2] = scalingVec.z;
	transformationMatrix[3][3] = 1.0f;
}

Rotation::Rotation(float angleRad_, const Vec3f& axis_)
	: angleRad(angleRad_), axis(axis_)
{
	// Calculate transformation matrix
	Vec3f n = axis.unitVector();
	float cosAlpha = cos(angleRad);
	float sinAlpha = sin(angleRad);
	float K = 1 - cosAlpha;
	float a = n.x;
	float b = n.y;
	float c = n.z;

	transformationMatrix[0][0] = cosAlpha + a*a*K;
	transformationMatrix[0][1] = a*b*K - c*sinAlpha;
	transformationMatrix[0][2] = a*c*K + b*sinAlpha;
	transformationMatrix[1][0] = a*b*K + c*sinAlpha;
	transformationMatrix[1][1] = cosAlpha + b*b*K;
	transformationMatrix[1][2] = b*c*K - a*sinAlpha;
	transformationMatrix[2][0] = a*c*K - b*sinAlpha;
	transformationMatrix[2][1] = b*c*K + a*sinAlpha;
	transformationMatrix[2][2] = cosAlpha + c*c*K;
	transformationMatrix[3][3] = 1.0f;
}

Composite::Composite(float elements_[])
{
	transformationMatrix = Matrix4f();

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			transformationMatrix[i][j] = elements_[i * 4 + j];
		}
	}
}