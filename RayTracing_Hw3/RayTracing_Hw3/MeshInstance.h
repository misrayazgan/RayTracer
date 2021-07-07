#ifndef MESHINSTANCE_H_
#define MESHINSTANCE_H_

#include "Object.h"

class MeshInstance : public Object
{
public:
	Object* baseMeshBVH;

	MeshInstance() {}
	MeshInstance(const Scene* scene_, int materialId_, Object* baseMeshBVH_, const Matrix4f& matrix_, bool transform_, const Vec3f& motionVector_, bool motion_);
	bool intersection(const Ray& ray, Hit& hit);
	~MeshInstance();
};

#endif
