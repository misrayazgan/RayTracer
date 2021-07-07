#include "Camera.h"

void Camera::SetCameraParams()
{
	gaze = gaze.unitVector();
	w = -1 * gaze;
	u = v.crossProduct(w).unitVector();
	v = w.crossProduct(u).unitVector();

	m = position - w * distance;
	q = m + left * u + top * v;
}