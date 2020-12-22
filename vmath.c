/* Project ........ Python Game Engine
 * Filename ....... vmath.c
 * Description .... Defines vector math operations.
 * Created by ..... Thomas Bellucci
 * Date ........... Dec 20th, 2020
 */ 

#include <vector>
#include <math.h>
#include <algorithm>
#include "types.h"
using namespace std;


/* 1D Definitions */


float min3f(float x, float y, float z){
    return min(min(x, y), z);
}

float max3f(float x, float y, float z) {
    return max(max(x, y), z);
}


/* 2D Definitions */


Vec2D scaleV2(Vec2D v, float scalar) {
    v.s = v.s * scalar;
    v.t = v.t * scalar;
    return v;
}


/* 3D Definitions */


Vec3D subtractV3(Vec3D v1, Vec3D v2) {
    Vec3D v3;
    v3.x = v1.x - v2.x;
    v3.y = v1.y - v2.y;
    v3.z = v1.z - v2.z;
    return v3;
}

Vec3D addV3(Vec3D v1, Vec3D v2) {
    Vec3D v3;
    v3.x = v1.x + v2.x;
    v3.y = v1.y + v2.y;
    v3.z = v1.z + v2.z;
    return v3;
}

float dot_prodV3(Vec3D v1, Vec3D v2) {
    return (v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z);
}

Vec3D normalizeV3(Vec3D v) {
    float scalar = 1 / sqrt(dot_prodV3(v, v));
    v.x = v.x * scalar;
    v.y = v.y * scalar;
    v.z = v.z * scalar;
    return v;
}

Vec3D scaleV3(Vec3D v, float scalar) {
    v.x = v.x * scalar;
    v.y = v.y * scalar;
    v.z = v.z * scalar;
    return v;
}

Vec3D add_scalarV3(Vec3D v, float scalar) {
    v.x = v.x + scalar;
    v.y = v.y + scalar;
    v.z = v.z + scalar;
    return v;
}

Vec3D clipV3(Vec3D v, float thres) {
    v.x = min(v.x, thres);
    v.y = min(v.y, thres);
    v.z = min(v.z, thres);
    return v;
}
