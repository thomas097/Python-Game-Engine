#ifndef _RASTERIZATION_H_
#define _RASTERIZATION_H_
#include <math.h>
#include "types.h"
#include <cstdio>
#include <vector>
using namespace std;


int is_backface(Eigen::Vector3f normal);

unsigned int texture_lookup(Texture* tex, float s, float t);

float f(float x0, float y0, float x1, float y1, float x, float y);

void rasterize_mesh_triangle(Camera* cam, Eigen::MatrixXf* v, Eigen::MatrixXf* vn, Eigen::MatrixXf* vt, Tri tri, Texture* tex);

void rasterize_mesh(Camera* cam, Mesh* mesh, Texture* tex);

#endif
