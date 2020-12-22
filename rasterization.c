/* Project ........ Python Game Engine
 * Filename ....... rasterization.c
 * Description .... Includes all functions used for rasterization.
 * Created by ..... Thomas Bellucci
 * Date ........... Dec 20th, 2020
 */  
#include <math.h>
#include <cstdio>
#include <algorithm>
#include <vector>
#include "vmath.h"
#include "shading.h"
#include "types.h"
#include "utilities.h"
using namespace std;


float f(float x0, float y0, float x1, float y1, float x, float y) {
    return (y0 - y1) * x + (x1 - x0) * y + x0 * y1 - x1 * y0;
}


void rasterize_triangle(Frame frame, Tri tri, Material mat, vector<Light> lights) {
    // Unpack vertex coordinates.
    float x0 = tri.vert0.v.x;
    float y0 = tri.vert0.v.y;
    float z0 = tri.vert0.v.z;
    
    float x1 = tri.vert1.v.x;
    float y1 = tri.vert1.v.y;
    float z1 = tri.vert1.v.z;
    
    float x2 = tri.vert2.v.x;
    float y2 = tri.vert2.v.y;
    float z2 = tri.vert2.v.z;
    
    // Unpack texture coordinates.
    float s0 = tri.vert0.vt.s;
    float t0 = tri.vert0.vt.t;

    float s1 = tri.vert1.vt.s;
    float t1 = tri.vert1.vt.t;
    
    float s2 = tri.vert2.vt.s;
    float t2 = tri.vert2.vt.t;
    
    // Unpack normals.
    float nx0 = tri.vert0.vn.x;
    float ny0 = tri.vert0.vn.y;
    float nz0 = tri.vert0.vn.z;
    
    float nx1 = tri.vert1.vn.x;
    float ny1 = tri.vert1.vn.y;
    float nz1 = tri.vert1.vn.z;
    
    float nx2 = tri.vert2.vn.x;
    float ny2 = tri.vert2.vn.y;
    float nz2 = tri.vert2.vn.z;
     
    // Determine bounding box for triangle.
    int x_min = floor(min3f(x0, x1, x2));
    int x_max =  ceil(max3f(x0, x1, x2));
    int y_min = floor(min3f(y0, y1, y2));
    int y_max =  ceil(max3f(y0, y1, y2));
    
    // Pre-compute f values.
    float alpha, beta, gamma;
    float fa = f(x1, y1, x2, y2, x0, y0);
    float fb = f(x2, y2, x0, y0, x1, y1);
    float fg = f(x0, y0, x1, y1, x2, y2);
    
    // Precompute whether fa and f for offscreen point is the same.
    float fa_off = fa * f(x1, y1, x2, y2, -1, -1) > 0;
    float fb_off = fb * f(x2, y2, x0, y0, -1, -1) > 0;
    float fg_off = fg * f(x0, y0, x1, y1, -1, -1) > 0;
    
    // Precompute updates in x- and y-direction
    float d12_x_update = (y1 - y2) / fa; 
    float d20_x_update = (y2 - y0) / fb;
    float d12_y_update = (x2 - x1) / fa; 
    float d20_y_update = (x0 - x2) / fb;
    
    // Precompute f12 and f20 as s12 and d20 respectively.
    float d12 = f(x1, y1, x2, y2, x_min, y_min) / fa;
    float d20 = f(x2, y2, x0, y0, x_min, y_min) / fb;
    
    int y, x, i, in_triangle;
    float z, s, t;
    Vec3D pixel, v, vn;
    Vec2D vt;
    Vertex vertex;
    
    for (y = y_min; y <= y_max; y++) {
        
        // Update barycentric coords alpha & beta.
        alpha = d12;
      	beta = d20;
      	
      	in_triangle = 0;
      	
        for (x = x_min; x <= x_max; x++) {
        
            // Perform border test.
            if (x < 0 || y < 0 || x >= frame.width || y >= frame.height) {
                continue;
            }
        
            // Update barycentric coord gamma.
            gamma = 1.0f - (alpha + beta);
            
            if (alpha >= 0 && beta >= 0 && gamma >= 0) {
                if ((alpha > 0 || fa_off) && (beta > 0 || fb_off) && (gamma > 0 || fg_off)) {
                    // Set in_triangle flag.
                    in_triangle = 1;
                    
                    // Compute vertex depth.
                    z = z0 * alpha + z1 * beta + z2 * gamma;
                 
                    // Perform depth test.
                    i = frame.width * y + x; 
                    if (z < frame.depth_buffer[i]) {
                    
                        // Compute vertex/pixel location.
                        v.x = alpha * x0 + beta * x1 + gamma * x2;
                        v.y = alpha * y0 + beta * y1 + gamma * y2;
                        v.z = alpha * z0 + beta * z1 + gamma * z2;
                    
                        // Compute texture coordinate.
                        vt.s = alpha * s0 + beta * s1 + gamma * s2;
                        vt.t = alpha * t0 + beta * t1 + gamma * t2;
                        
                        // Compute vertex normal.
                        vn.x = alpha * nx0 + beta * nx1 + gamma * nx2;
                        vn.y = alpha * ny0 + beta * ny1 + gamma * ny2;
                        vn.z = alpha * nz0 + beta * nz1 + gamma * nz2;
                        
                        // Put into new vertex.
                        vertex.v = v;
                        vertex.vt = vt;
                        vertex.vn = vn;
                        
                        // Fill in pixel with correct color.
                        pixel = shade_vertex(vertex, lights, mat);
                        frame.frame_buffer[3 * i] = pixel.x;
                        frame.frame_buffer[3 * i + 1] = pixel.y;
                        frame.frame_buffer[3 * i + 2] = pixel.z;
                        
                        // Update depth buffer.
                        frame.depth_buffer[i] = z;
                    }
                }
                
            // Triangle exit test.
            } else if (in_triangle) {
                break;
            }
            
            // Update barycentric coords.
            alpha += d12_x_update;
            beta += d20_x_update;
        }
        // Update barycentric coords.
        d12 += d12_y_update;
  	    d20 += d20_y_update;
    }
    return; 
}

void rasterize_object(Frame frame, Object obj, vector<Light> lights) {
    unsigned long size = obj.mesh.size();
    for (unsigned long i = 1; i < size; i++) {
        rasterize_triangle(frame, obj.mesh[i], obj.mat, lights);
    }        
}

