#ifndef MATERIAL_H_
#define MATERIAL_H_

#include "Vec3f.h"

struct Material
{
    Vec3f ambient;
    Vec3f diffuse;
    Vec3f specular;
    Vec3f mirror;
    float phongExponent;
};

#endif