#ifndef OBJECT_H_
#define OBJECT_H_

#include "Ray.h"
#include "BoundingBox.h"

const float epsilon = 0.000001f;
class Scene;

class Object
{
public:
	int materialId;

	Object() {}
	Object(const Scene* scene_, int id_) : scene(scene_), materialId(id_) {}
	virtual bool intersection(const Ray& ray, Hit& hit) = 0;
	const BoundingBox& getBoundingBox() const
	{
		return boundingBox;
	}

protected:
	const Scene* scene;
	BoundingBox boundingBox;
};

#endif