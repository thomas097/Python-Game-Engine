#ifndef _CAMERA_TRANSFORMS_H_
#define _CAMERA_TRANSFORMS_H_
#include <Eigen/Dense>
#include <iostream>


void transform_vertices(Eigen::MatrixXf* dst, Eigen::Matrix4f M, Eigen::MatrixXf vertices);

Eigen::Matrix4f* camera_transform(Eigen::Vector3f origin, Eigen::Vector3f direction);

Eigen::Matrix4f* viewport_transform(unsigned int frame_width, unsigned int frame_height, float fov, float n, float f);


#endif





