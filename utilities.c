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


int is_tri(const char *string) {
    int size = strlen(string);
    // Count spaces.
    int count = 0;
    for (int i = 0; i < size; i++) {
        int ch = (int) (string[i]);
        if (ch == 32) count++;
    }
    return count == 3;
}


// Imports geometry as a vector of Tris from an OBJ file.
vector<Tri> load_mesh(const char filename[]) {
    char line[512];
    int v0, v1, v2, v3, vt0, vt1, vt2, vt3, vn0, vn1, vn2, vn3;

    // Dynamic arrays to store geometry.    
    vector<Vec3D> vertices, normals;
    vector<Vec2D> texcoords;
    vector<Tri> faces;

    // Loop through lines in file.
    FILE *file = fopen(filename, "r");
    while(fgets(line, 512, file)) {

        // Vertex coordinates.
        if (strncmp(line, "v ", 2) == 0) {
            Vec3D vertex;
            sscanf(line, "v %f %f %f", &vertex.x, &vertex.y, &vertex.z);
            vertices.push_back(vertex);
        }
        
        // Texture coordinates.
        else if (strncmp(line, "vt ", 3) == 0) {
            Vec2D texcoord;
            sscanf(line, "vt %f %f", &texcoord.s, &texcoord.t);
            texcoords.push_back(texcoord);
        }
        
        // Normals.
        else if (strncmp(line, "vn ", 3) == 0) {
            Vec3D normal;
            sscanf(line, "vn %f %f %f", &normal.x, &normal.y, &normal.z);
            normals.push_back(normal);
        }
        
        // Faces (defined as v/vt/vn triples).
        else if (strncmp(line, "f ", 2) == 0) {
        
            // Parse triangles.
            if (is_tri(line)) {
                sscanf(line, "f %d/%d/%d %d/%d/%d %d/%d/%d", &v0, &vt0, &vn0, &v1, &vt1, &vn1, &v2, &vt2, &vn2);
                
                Vertex vertex0;
                vertex0.v = vertices[v0 - 1];
                vertex0.vt = texcoords[vt0 - 1];
                vertex0.vn = normals[vn0 - 1];
                
                Vertex vertex1;
                vertex1.v = vertices[v1 - 1];
                vertex1.vt = texcoords[vt1 - 1];
                vertex1.vn = normals[vn1 - 1];
                
                Vertex vertex2;
                vertex2.v = vertices[v2 - 1];
                vertex2.vt = texcoords[vt2 - 1];
                vertex2.vn = normals[vn2 - 1];
                
                Tri tri;
                tri.vertex0 = vertex0;
                tri.vertex1 = vertex1;
                tri.vertex2 = vertex2;
                faces.push_back(tri);
            } 
            // Quads
            else {
                sscanf(line, "f %d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d", &v0, &vt0, &vn0, &v1, &vt1, &vn1, &v2, &vt2, &vn2, &v3, &vt3, &vn3);
                
                // Define vertices of corners.
                Vertex vertex0;
                vertex0.v = vertices[v0 - 1];
                vertex0.vt = texcoords[vt0 - 1];
                vertex0.vn = normals[vn0 - 1];
                
                Vertex vertex1;
                vertex1.v = vertices[v1 - 1];
                vertex1.vt = texcoords[vt1 - 1];
                vertex1.vn = normals[vn1 - 1];
                
                Vertex vertex2;
                vertex2.v = vertices[v2 - 1];
                vertex2.vt = texcoords[vt2 - 1];
                vertex2.vn = normals[vn2 - 1];
                
                Vertex vertex3;
                vertex3.v = vertices[v3 - 1];
                vertex3.vt = texcoords[vt3 - 1];
                vertex3.vn = normals[vn3 - 1];
                
                Tri tri0;
                tri0.vertex0 = vertex0;
                tri0.vertex1 = vertex1;
                tri0.vertex2 = vertex2;
                faces.push_back(tri0);
                
                Tri tri1;
                tri1.vertex0 = vertex2;
                tri1.vertex1 = vertex0;
                tri1.vertex2 = vertex3;
                faces.push_back(tri1);
            }  
        }
    }
    fclose(file);
    return faces;
}

Texture load_texture(const char filename[]) {
    int width, height, bpp;
    uint8_t* data = stbi_load(filename, &width, &height, &bpp, 3);

    Texture tex;
    tex.width = width;
    tex.height = height;
    tex.data = data;
	return tex;
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
