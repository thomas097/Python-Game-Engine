#ifndef _TYPES_H_
#define _TYPES_H_

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

#endif
