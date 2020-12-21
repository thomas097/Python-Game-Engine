/* Project ........ Python Game Engine
 * Filename ....... utilities.c
 * Description .... Defines various utility functions, incl. I/O functions.
 * Created by ..... Thomas Bellucci
 * Date ........... Dec 20th, 2020
 */ 

#include <iostream> 
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <vector> 
#include "types.h"
using namespace std;


// Returns True when string starts with prefix.
int StartsWith(const char *string, const char *prefix) {
    if(strncmp(string, prefix, strlen(prefix)) == 0) {
        return 1;
    }
    return 0;
}


vector<Tri> ImportOBJ(const char filename[]) {
    char line[512];
    float x, y, z, s, t;
    int i_v, i_vt, i_vn;
    int v0, v1, v2, vt0, vt1, vt2, vn0, vn1, vn2;

    // Dynamic arrays to store geometry.    
    vector<Vec3D> v, vn;
    vector<Vec2D> vt;
    vector<Tri> f;

    // Loop through lines in file.
    FILE *file = fopen(filename, "r");
    while(fgets(line, 512, file)) {

        // Vertex coordinates.
        if (StartsWith(line, "v ")) {
            Vec3D vertex;
            sscanf(line, "v %f %f %f", &vertex.x, &vertex.y, &vertex.z);
            v.push_back(vertex);
        }
        
        // Texture coordinates.
        else if (StartsWith(line, "vt ")) {
            Vec2D texcoords;
            sscanf(line, "vt %f %f", &texcoords.s, &texcoords.t);
            vt.push_back(texcoords);
        }
        
        // Vertex normals.
        else if (StartsWith(line, "vn ")) {
            Vec3D normal;
            sscanf(line, "vn %f %f %f", &normal.x, &normal.y, &normal.z);
            vn.push_back(normal);
        }
        
        // Triangulated faces (defined as three v/vt/vn triples).
        else if (StartsWith(line, "f ")) {
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
    }
    fclose(file);
    return f;
}
