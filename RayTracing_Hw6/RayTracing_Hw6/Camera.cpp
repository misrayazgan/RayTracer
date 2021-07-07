#include "Camera.h"

void Camera::setCameraParams()
{
	gaze = gaze.unitVector();
	w = -1 * gaze;
	u = v.crossProduct(w).unitVector();
	v = w.crossProduct(u).unitVector();

	if (orientation == ORIENTATION_LEFTHANDED)
	{
		u = -1 * u;
	}

	m = position - w * distance;
	q = m + left * u + top * v;
}

bool Camera::hasDepthOfField() const
{
	// If aperture size is zero, return false.
	return apertureSize;
}