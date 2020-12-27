#ifndef _CAMERA_TRANSFORMS_H_
#define _CAMERA_TRANSFORMS_H_
#include <Eigen/Dense>
#include <iostream>


Eigen::Matrix4f* camera_transform(Eigen::Vector3f origin, Eigen::Vector3f direction);

Eigen::Matrix4f* viewport_transform(unsigned int frame_width, unsigned int frame_height, float fov, float n, float f);

Camera create_camera(int frame_width, int frame_height, Eigen::Vector3f origin, Eigen::Vector3f direction, float fov, float min_draw_dist, float max_draw_dist);

void move_camera(Camera* cam, Eigen::Vector3f origin, Eigen::Vector3f direction);

Camera reset_camera(Camera cam);



#endif





