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
#include "types.h"
#include "utilities.h"
using namespace std;


#define NUM_THREADS 4


int is_backface(Tri tri) {
    float z_dir = tri.vertex0.vn.z + tri.vertex1.vn.z + tri.vertex2.vn.z;
    return z_dir > 0;
}


Vec3D texture_lookup(Texture* tex, float s, float t) {
    int width = tex->width;
    int height = tex->height;
    
    // Nearest neighbor interpolation.
    int u = floor(width * s + .5);
    int v = floor(height * (1 - t) + .5);
    
    // Repeat texture.
    u = u % width;
    v = v % height;
    if (u < 0) width - u;
    if (v < 0) width - v;
    
    // Put pixel in vector.
    Vec3D pixel;
    int i = 3 * (v * width + u);
    pixel.x = tex->data[i];
    pixel.y = tex->data[i + 1];
    pixel.z = tex->data[i + 2];
    return pixel;
}


float f(float x0, float y0, float x1, float y1, float x, float y) {
    return (y0 - y1) * x + (x1 - x0) * y + x0 * y1 - x1 * y0;
}


void rasterize_triangle(Frame* frame, Tri &tri, Texture* tex) {
    // Backface culling
    if (is_backface(tri)) return;
    
    // Unpack vertex coordinates.
    float x0 = tri.vertex0.v.x;
    float y0 = tri.vertex0.v.y;
    float z0 = tri.vertex0.v.z;
    
    float x1 = tri.vertex1.v.x;
    float y1 = tri.vertex1.v.y;
    float z1 = tri.vertex1.v.z;
    
    float x2 = tri.vertex2.v.x;
    float y2 = tri.vertex2.v.y;
    float z2 = tri.vertex2.v.z;
    
    // Unpack texture coordinates.
    float s0 = tri.vertex0.vt.s;
    float t0 = tri.vertex0.vt.t;

    float s1 = tri.vertex1.vt.s;
    float t1 = tri.vertex1.vt.t;
    
    float s2 = tri.vertex2.vt.s;
    float t2 = tri.vertex2.vt.t;
     
    // Determine bounding box for triangle.
    int x_min = floor(min(x0, min(x1, x2)));
    int x_max = ceil(max(x0, max(x1, x2)));
    int y_min = floor(min(y0, min(y1, y2)));
    int y_max = ceil(max(y0, max(y1, y2)));
    
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
    float alpha_x_update = (y1 - y2) / fa; 
    float beta_x_update = (y2 - y0) / fb;
    float alpha_y_update = (x2 - x1) / fa; 
    float beta_y_update = (x0 - x2) / fb;
    
    // Precompute f12 and f20 as s12 and d20 respectively.
    float alpha_init = f(x1, y1, x2, y2, x_min, y_min) / fa;
    float beta_init = f(x2, y2, x0, y0, x_min, y_min) / fb;
    
    int frame_width = frame->width;
    int frame_height = frame->height;
    
    int y, x, i, in_triangle;
    float z, s, t;
    Vec3D pixel;
    
    for (y = y_min; y <= y_max; y++) {
        
        // Update barycentric coords alpha & beta.
        alpha = alpha_init;
      	beta = beta_init;
      	
      	in_triangle = 0;
      	
        for (x = x_min; x <= x_max; x++) {
        
            // Perform border test.
            if (x < 0 || y < 0 || x > frame_width || y > frame_height) {
                continue;
            }
        
            // Update barycentric gamma.
            gamma = 1.0f - (alpha + beta);
            
            if (alpha >= 0 && beta >= 0 && gamma >= 0) {
                if ((alpha > 0 || fa_off) && (beta > 0 || fb_off) && (gamma > 0 || fg_off)) {
                    // Set in_triangle flag.
                    in_triangle = 1;
                    
                    // Compute vertex depth.
                    z = z0 * alpha + z1 * beta + z2 * gamma;
                 
                    // Perform depth test.
                    i = frame->width * y + x; 
                    if (z < frame->depth_buffer[i]) {
                    
                        // Compute texture coordinate.
                        s = alpha * s0 + beta * s1 + gamma * s2;
                        t = alpha * t0 + beta * t1 + gamma * t2;
                        
                        // Fill in pixel with correct color.
                        pixel = texture_lookup(tex, s, t);
                        frame->frame_buffer[3 * i] = pixel.x;
                        frame->frame_buffer[3 * i + 1] = pixel.y;
                        frame->frame_buffer[3 * i + 2] = pixel.z;
                        
                        // Update depth buffer.
                        frame->depth_buffer[i] = z;
                    }
                }
                
            // Triangle exit test.
            } else if (in_triangle) {
                break;
            }
            
            // Update barycentric coords.
            alpha += alpha_x_update;
            beta += beta_x_update;
        }
        alpha_init += alpha_y_update;
  	    beta_init += beta_y_update;
    }
    return; 
}


void rasterize_object(Frame* frame, vector<Tri>* mesh, Texture* tex) {
    unsigned long size = mesh->size();
    for (unsigned long i = 0; i < size; i++) {
        rasterize_triangle(frame, (*mesh)[i], tex);
    }
}






