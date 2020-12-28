#ifndef _TYPES_H_
#define _TYPES_H_
#include <vector>
#include <stdint.h>
#include <Eigen/Dense>

struct Tri {
    unsigned int iv0;
    unsigned int iv1;
    unsigned int iv2;
    
    unsigned int ivt0;
    unsigned int ivt1;
    unsigned int ivt2;
    
    unsigned int ivn0;
    unsigned int ivn1;
    unsigned int ivn2;
};

struct Mesh {
    Eigen::MatrixXf* v;
    Eigen::MatrixXf* vn;
    Eigen::MatrixXf* vt;
    std::vector<Tri>* f;
};

struct Texture {
	int width;
	int height;
	uint8_t* data;
};

struct Camera {
    Eigen::Matrix4f* Mvp;
    Eigen::Matrix4f* Mcam;
    Eigen::Vector3f* origin;
    Eigen::Vector3f* direction;
    int frame_width;
    int frame_height;
    unsigned char* frame_buffer;
    double* depth_buffer;
    double min_draw_dist;
    double max_draw_dist;
};


#endif
