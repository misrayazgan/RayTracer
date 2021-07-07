#ifndef CAMERA_H_
#define CAMERA_H_

#include "Vec3f.h"
#include <string>

class Camera
{
public:
	Vec3f position;
	Vec3f gaze;
	Vec3f v;	// up vector
	Vec3f u;	// side vector
	Vec3f w;	// -gaze
	Vec3f m;	// center of the image plane
	Vec3f q;	// top left corner
	float left, right, bottom, top;	// image plane
	float distance;	// between camera to image plane
	int imageWidth;
	int imageHeight;
	std::string imageName;

	Camera() {}
	Camera(Vec3f p, Vec3f g, Vec3f v, float l, float r, float b, float t, float d, int w, int h, std::string n)
		: position(p), gaze(g), v(v), left(l), right(r), bottom(b), top(t), distance(d), imageWidth(w), imageHeight(h), imageName(n) {}
	void SetCameraParams();
};

#endif