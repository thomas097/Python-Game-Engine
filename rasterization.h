#ifndef _RASTERIZATION_H_
#define _RASTERIZATION_H_
#include <math.h>
#include "types.h"
#include <cstdio>
#include <vector>
#include <Eigen/Dense>
using namespace std;


int is_backface(Eigen::Vector3f normal);

double f(Eigen::Vector3f v0, Eigen::Vector3f v1, double x, double y);

void rasterize_mesh_triangle(Camera* cam, Eigen::MatrixXf* v, Eigen::MatrixXf* vn, Eigen::MatrixXf* vt, Tri tri, Texture* texture);

void rasterize_mesh(Camera* cam, Mesh* mesh, Texture* texture);

#endif
