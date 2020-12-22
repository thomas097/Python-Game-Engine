/* Project ........ Python Game Engine
 * Filename ....... utilities.c
 * Description .... Defines various utility functions, incl. I/O functions.
 * Created by ..... Thomas Bellucci
 * Date ........... Dec 20th, 2020
 */ 

#include <iostream>
#include <fstream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <vector> 
#include <stdint.h>
#include "types.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
using namespace std;


// Determines whether OBJ row is a quad.
int is_quad(const char *string) {
    int size = strlen(string);
    // Count spaces.
    int count = 0;
    for (int i = 0; i < size; i++) {
        int ch = (int) (string[i]);
        if (ch == 32) count++;
    }
    return count == 4;
}


// Imports geometry as a vector of Tris from an OBJ file.
vector<Tri> import_OBJ(const char filename[]) {
    char line[512];
    float x, y, z, s, t;
    int i_v, i_vt, i_vn;
    int v0, v1, v2, v3, vt0, vt1, vt2, vt3, vn0, vn1, vn2, vn3;

    // Dynamic arrays to store geometry.    
    vector<Vec3D> v, vn;
    vector<Vec2D> vt;
    vector<Tri> f;

    // Loop through lines in file.
    FILE *file = fopen(filename, "r");
    while(fgets(line, 512, file)) {

        // Vertex coordinates.
        if (strncmp(line, "v ", 2) == 0) {
            Vec3D vertex;
            sscanf(line, "v %f %f %f", &vertex.x, &vertex.y, &vertex.z);
            v.push_back(vertex);
        }
        
        // Texture coordinates.
        else if (strncmp(line, "vt ", 3) == 0) {
            Vec2D texcoords;
            sscanf(line, "vt %f %f", &texcoords.s, &texcoords.t);
            vt.push_back(texcoords);
        }
        
        // Vertex normals.
        else if (strncmp(line, "vn ", 3) == 0) {
            Vec3D normal;
            sscanf(line, "vn %f %f %f", &normal.x, &normal.y, &normal.z);
            vn.push_back(normal);
        }
        
        // Faces (defined as v/vt/vn triples).
        else if (strncmp(line, "f ", 2) == 0) {
        
            // Triangles.
            if (!is_quad(line)) {
                sscanf(line, "f %d/%d/%d %d/%d/%d %d/%d/%d", &v0, &vt0, &vn0, &v1, &vt1, &vn1, &v2, &vt2, &vn2);
                
                // Define vertices of corners.
                Vertex vertex0;
                vertex0.v = v[v0 - 1];
                vertex0.vt = vt[vt0 - 1];
                vertex0.vn = vn[vn0 - 1];
                
                Vertex vertex1;
                vertex1.v = v[v1 - 1];
                vertex1.vt = vt[vt1 - 1];
                vertex1.vn = vn[vn1 - 1];
                
                Vertex vertex2;
                vertex2.v = v[v2 - 1];
                vertex2.vt = vt[vt2 - 1];
                vertex2.vn = vn[vn2 - 1];
                
                // Combine vertices into a Tri struct.
                Tri triangle;
                triangle.vert0 = vertex0;
                triangle.vert1 = vertex1;
                triangle.vert2 = vertex2;
                f.push_back(triangle);
            } 
            // Quads
            else {
                sscanf(line, "f %d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d", &v0, &vt0, &vn0, &v1, &vt1, &vn1, &v2, &vt2, &vn2, &v3, &vt3, &vn3);
                
                // Define vertices of corners.
                Vertex vertex0;
                vertex0.v = v[v0 - 1];
                vertex0.vt = vt[vt0 - 1];
                vertex0.vn = vn[vn0 - 1];
                
                Vertex vertex1;
                vertex1.v = v[v1 - 1];
                vertex1.vt = vt[vt1 - 1];
                vertex1.vn = vn[vn1 - 1];
                
                Vertex vertex2;
                vertex2.v = v[v2 - 1];
                vertex2.vt = vt[vt2 - 1];
                vertex2.vn = vn[vn2 - 1];
                
                Vertex vertex3;
                vertex3.v = v[v3 - 1];
                vertex3.vt = vt[vt3 - 1];
                vertex3.vn = vn[vn3 - 1];
                
                // Combine Quad vertices into two Tri structs.
                Tri triangle1;
                triangle1.vert0 = vertex2;
                triangle1.vert1 = vertex0;
                triangle1.vert2 = vertex3;
                f.push_back(triangle1);
                
                Tri triangle0;
                triangle0.vert0 = vertex0;
                triangle0.vert1 = vertex1;
                triangle0.vert2 = vertex2;
                f.push_back(triangle0);
            }  
        }
    }
    fclose(file);
    return f;
}


Frame create_frame(int width, int height) {
    Frame frame;
    frame.width = width;
    frame.height = height;
    frame.frame_buffer = new unsigned char[width * height * 3];
    frame.depth_buffer = new float[width * height];
    return frame;
}


Frame clear_frame(Frame frame, float max_depth) {
    unsigned long size = frame.width * frame.height;
    memset(frame.frame_buffer, 0, size * 3);
    for (unsigned long i = 0; i < size; i++){
        frame.depth_buffer[i] = max_depth;
    }
    return frame;
}


Object create_object(const char obj_file[], Material mat) {
    Object obj;
    obj.mesh = import_OBJ(obj_file);
    obj.mat = mat;
    return obj;
}


Texture create_texture(const char filename[]) {
    int width, height, bpp;
    uint8_t* data = stbi_load(filename, &width, &height, &bpp, 3);

    Texture tex;
    tex.width = width;
    tex.height = height;
    tex.data = data;
	return tex;
}


Material create_material(const char texture[], float fresnel) {
    Material mat;
    mat.tex = create_texture(texture);
    mat.fresnel = fresnel;
    return mat;
}


Light create_light(vector<float> loc, float intensity) {
    Light light;
    light.intensity = intensity;
    Vec3D v;
    v.x = loc[0];
    v.y = loc[1];
    v.z = loc[2];
    light.loc = v;
    return light;
}

Vec3D create_Vec3D(int x, int y, int z) {
    Vec3D vec;
    vec.x = x;
    vec.y = y;
    vec.z = z;
    return vec;
}
