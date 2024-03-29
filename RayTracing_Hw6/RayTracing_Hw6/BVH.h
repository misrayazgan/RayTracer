#ifndef BVH_H_
#define BVH_H_

#include "Object.h"
#include <vector>


class BVH : public Object
{
public:
	Object* left;				// pointer to left child
	Object* right;				// pointer to right child
	// bounding box for the current BVH node is derived from base class Object

	BVH(std::vector<Object*>& objects, int start, int end, int axis);
	bool intersection(const Ray& ray, Hit& hit);
	~BVH();
};



#endif