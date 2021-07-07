#ifndef OBJECT_H_
#define OBJECT_H_

#include "Ray.h"

class Scene;

class Object
{
public:
	int materialId;

	Object(const Scene* scene_, int id_) : scene(scene_), materialId(id_) {}
	virtual Hit intersection(const Ray& ray) = 0;

protected:
	const Scene* scene;
};

#endif