#ifndef _RASTERIZATION_H_
#define _RASTERIZATION_H_
#include <math.h>
#include "types.h"
#include <cstdio>
#include <vector>
using namespace std;


int is_backface(Tri tri);

Vec3D texture_lookup(Texture tex, float s, float t);

float f(float x0, float y0, float x1, float y1, float x, float y);

void rasterize_triangle(Frame* frame, Tri &tri, Texture* tex);

void rasterize_object(Frame* frame, vector<Tri>* mesh, Texture* tex);

#endif
