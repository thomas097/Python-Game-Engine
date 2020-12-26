/* Project ........ Python Game Engine
 * Filename ....... imports.c
 * Description .... Defines various utility functions, incl. I/O functions.
 * Created by ..... Thomas Bellucci
 * Date ........... Dec 20th, 2020
 */ 

#include <iostream>
#include <Eigen/Dense>
#include <fstream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <vector> 
#include <stdint.h>
#include "types.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


int count_vertices(const char *string)
{
    int size = strlen(string);
    // Count spaces.
    int count = 0;
    for (int i = 0; i < size; i++) {
        int ch = (int) (string[i]);
        if (ch == 32) count++;
    }
    return count;
}


Mesh load_mesh(const char filename[])
{
    // Compute number of vertices, normals and texture coordinates in mesh.
    unsigned long num_verts, num_texcoords, num_normals, num_faces;
    num_verts = num_normals = num_texcoords = num_faces = 0;
    
    char line[512];
    FILE *file = fopen(filename, "r");
    while(fgets(line, 512, file)) {
        if (strncmp(line, "v ", 2) == 0) num_verts++;
        if (strncmp(line, "vt ", 2) == 0) num_texcoords++;
        if (strncmp(line, "vn ", 2) == 0) num_normals++;
        if (strncmp(line, "f ", 2) == 0) num_faces += count_vertices(line) - 2;
    }
    fclose(file);
    
    printf("Loading %ld vertices...\n", num_verts);
    printf("Loading %ld triangle faces...\n", num_faces);

    // Define buffers to store geometry (in homogeneous coordinates).
    Eigen::MatrixXf* vertices = new Eigen::MatrixXf(4, num_verts);
    Eigen::MatrixXf* normals = new Eigen::MatrixXf(4, num_normals);
    Eigen::MatrixXf* texcoords = new Eigen::MatrixXf(2, num_texcoords);
    std::vector<Tri>* faces = new std::vector<Tri>;
    
    // Init temporary indices for data.
    int v0, v1, v2, v3, vt0, vt1, vt2, vt3, vn0, vn1, vn2, vn3;
    unsigned long i_v, i_vn, i_vt;
    i_v = i_vn = i_vt = 0;
    float x, y, z, u, v;

    // Loop through lines in file.
    file = fopen(filename, "r");
    while(fgets(line, 512, file)) {

        // Vertex coordinates.
        if (strncmp(line, "v ", 2) == 0) {
            sscanf(line, "v %f %f %f", &x, &y, &z);
            (*vertices)(0, i_v) = x;
            (*vertices)(1, i_v) = y;
            (*vertices)(2, i_v) = z;
            (*vertices)(3, i_v) = 1;
            i_v++;
        }
        
        // Texture coordinates.
        else if (strncmp(line, "vt ", 3) == 0) {
            sscanf(line, "vt %f %f", &u, &v);
            (*texcoords)(0, i_vt) = u;
            (*texcoords)(1, i_vt) = v;
            i_vt++;
        }
        
        // Normals.
        else if (strncmp(line, "vn ", 3) == 0) {
            sscanf(line, "vn %f %f %f", &x, &y, &z);
            (*normals)(0, i_vn) = x;
            (*normals)(1, i_vn) = y;
            (*normals)(2, i_vn) = z;
            (*normals)(3, i_vn) = 1;
            i_vn++;
        }
        
        // Faces (defined as v/vt/vn triples).
        else if (strncmp(line, "f ", 2) == 0) {
        
            // Parse triangles.
            if (count_vertices(line) == 3) {
                sscanf(line, "f %d/%d/%d %d/%d/%d %d/%d/%d", &v0, &vt0, &vn0, &v1, &vt1, &vn1, &v2, &vt2, &vn2);
                
                Tri tri;
                tri.iv0 = v0 - 1;
                tri.iv1 = v1 - 1;
                tri.iv2 = v2 - 1;
                
                tri.ivt0 = vt0 - 1;
                tri.ivt1 = vt1 - 1;
                tri.ivt2 = vt2 - 1;
                
                tri.ivn0 = vn0 - 1;
                tri.ivn1 = vn1 - 1;
                tri.ivn2 = vn2 - 1;
                faces->push_back(tri);
            } 
            // Parse quad as two triangles.
            else {
                sscanf(line, "f %d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d", &v0, &vt0, &vn0, &v1, &vt1, &vn1, &v2, &vt2, &vn2, &v3, &vt3, &vn3);

                Tri tri1;
                tri1.iv0 = v0 - 1;
                tri1.iv1 = v1 - 1;
                tri1.iv2 = v2 - 1;
                
                tri1.ivt0 = vt0 - 1;
                tri1.ivt1 = vt1 - 1;
                tri1.ivt2 = vt2 - 1;
                
                tri1.ivn0 = vn0 - 1;
                tri1.ivn1 = vn1 - 1;
                tri1.ivn2 = vn2 - 1;
                faces->push_back(tri1);
                
                Tri tri2;
                tri2.iv0 = v2 - 1;
                tri2.iv1 = v0 - 1;
                tri2.iv2 = v3 - 1;
                
                tri2.ivt0 = vt2 - 1;
                tri2.ivt1 = vt0 - 1;
                tri2.ivt2 = vt3 - 1;
                
                tri2.ivn0 = vn2 - 1;
                tri2.ivn1 = vn0 - 1;
                tri2.ivn2 = vn3 - 1;
                faces->push_back(tri2);
            }  
        }
    }
    fclose(file);
    printf("Loading completed!\n");
    
    Mesh mesh;
    mesh.v = vertices;
    mesh.vt = texcoords;
    mesh.vn = normals;
    mesh.f = faces;
    return mesh;
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

