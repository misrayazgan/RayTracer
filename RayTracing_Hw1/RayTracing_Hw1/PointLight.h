#ifndef POINTLIGHT_H_
#define POINTLIGHT_H_

#include "Vec3i.h"

struct PointLight
{
    Vec3f position;
    Vec3f intensity;
};

#endif