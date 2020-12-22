#ifndef _VMATH_H_
#define _VMATH_H_

#include <vector> 
#include "types.h"
using namespace std;

float min3f(float x, float y, float z);

float max3f(float x, float y, float z);

Vec2D scaleV2(Vec2D v, float scalar);

Vec3D subtractV3(Vec3D v1, Vec3D v2);

Vec3D addV3(Vec3D v1, Vec3D v2);

float dot_prodV3(Vec3D v1, Vec3D v2);

Vec3D normalizeV3(Vec3D v);

Vec3D scaleV3(Vec3D v, float scalar);

Vec3D add_scalarV3(Vec3D v, float scalar);

Vec3D clipV3(Vec3D v, float thres);

#endif
