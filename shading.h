#ifndef _SHADING_H_
#define _SHADING_H_
#include <Eigen/Dense>
#include <vector>
#include "types.h"

unsigned int texture_lookup(Eigen::Vector2f* texcoord);

void shade_pixel(Eigen::Vector3f* pixel, Eigen::Vector3f* vertex, Eigen::Vector3f* normal, Eigen::Vector2f* texcoord, Texture* texture);

#endif
