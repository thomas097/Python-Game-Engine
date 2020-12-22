#ifndef _SHADING_H_
#define _SHADING_H_

#include "types.h"


Vec3D texture_lookup(Texture tex, float s, float t);

Vec3D shade_vertex(Vertex vertex, std::vector<Light> lights, Material mat);

#endif
