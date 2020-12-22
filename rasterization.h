#ifndef _RASTERIZATION_H_
#define _RASTERIZATION_H_
#include <math.h>
#include "types.h"
#include <cstdio>
#include <vector>
using namespace std;

float f(float x0, float y0, float x1, float y1, float x, float y);

void rasterize_triangle(Frame frame, Tri tri, Material mat, vector<Light> lights);

void rasterize_object(Frame frame, Object obj, vector<Light> lights);

#endif
