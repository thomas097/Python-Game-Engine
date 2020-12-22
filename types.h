#ifndef _TYPES_H_
#define _TYPES_H_
#include <stdint.h>
#include <vector>
using namespace std;

struct Vec2D {
    float s;
    float t;
};

struct Vec3D {
    float x;
    float y;
    float z;
};

struct Vertex {
    Vec3D v;
    Vec2D vt;
    Vec3D vn;
};

struct Tri {
    Vertex vert0;
    Vertex vert1;
    Vertex vert2;
};

struct Texture {
	int width;
	int height;
	uint8_t* data;
};

struct Material {
    Texture tex; // Texture
    float fresnel;    // Fresnel
};

struct Light {
    float intensity; // Light intensity
    Vec3D loc;       // Location of light.

};

struct Object {
    vector<Tri> mesh;
    Material mat;
};

struct Frame {
    int width;
    int height;
    unsigned char* frame_buffer;
    float* depth_buffer;
};


#endif
