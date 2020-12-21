/* Project ........ Python Game Engine
 * Filename ....... rasterization.c
 * Description .... Includes all functions used for rasterization.
 * Created by ..... Thomas Bellucci
 * Date ........... Dec 20th, 2020
 */  
#include <math.h>
#include "types.h"
#include <cstdio>


// Computes the implicit line function f for triangle rasterization. 
float f(float x0, float y0, float x1, float y1, float x, float y) {
    return (y0 - y1) * x + (x1 - x0) * y + x0 * y1 - x1 * y0;
}

// Calculates the minumum of three floats. 
int min3f(float a, float b, float c) {
    int x_min;
    x_min = (a <= b) ? a : b;
    x_min = (x_min <= c) ? x_min : c;
    return x_min;
}

// Calculates the maximum of three floats.
int max3f(float a, float b, float c) {
    int x_max;
    x_max = (a > b) ? a : b;
    x_max = (x_max > c) ? x_max : c;
    return x_max;
}

// Rasterizes the triangle spanned by points pt0, pt1 and pt2 in frame buffer.
void RasterizeTriangle(int frame_width, int frame_height, unsigned char* frame_buffer, unsigned char* depth_buffer, Tri tri) {
    // Unpack coordinates from points.
    int x0 = tri.vert0.v.x;
    int y0 = tri.vert0.v.y;
    int z0 = tri.vert0.v.z;
    
    int x1 = tri.vert1.v.x;
    int y1 = tri.vert1.v.y;
    int z1 = tri.vert1.v.z;
    
    int x2 = tri.vert2.v.x;
    int y2 = tri.vert2.v.y;
    int z2 = tri.vert2.v.z;
     
    // Determine bounding box for triangle.
    int x_min = floor(min3f(x0, x1, x2));
    int x_max = ceil(max3f(x0, x1, x2));
    int y_min = floor(min3f(y0, y1, y2));
    int y_max = ceil(max3f(y0, y1, y2));
    
    // Pre-compute f values.
    float alpha, beta, gamma;
    float fa = f(x1, y1, x2, y2, x0, y0);
    float fb = f(x2, y2, x0, y0, x1, y1);
    float fg = f(x0, y0, x1, y1, x2, y2);
    
    // Precompute updates in x- and y-direction
    float d12_x_update = y1 - y2; 
    float d20_x_update = y2 - y0;
    float d12_y_update = x2 - x1; 
    float d20_y_update = x0 - x2;
    
    // Precompute f12 and f20 as s12 and d20 respectively.
    float d12 = f(x1, y1, x2, y2, x_min, y_min);
    float d20 = f(x2, y2, x0, y0, x_min, y_min);
    
    int y, x, i;
    float z;
    for (y = y_min; y <= y_max; y++) {
    
        float new_d12 = d12;
      	float new_d20 = d20;
      	
        for (x = x_min; x <= x_max; x++) {
        
            alpha = new_d12 / fa;
            beta = new_d20 / fb;
            gamma = 1.0 - (alpha + beta);
            
            if ((alpha > 0) && (beta > 0) & (gamma > 0)) {
                // Perform border test.
                if (x < 0 || y < 0 || x >= frame_width || y >= frame_height) {
                    continue;
                }
                
                // Perform depth test.
                i = frame_width * y + x;
                z = z0 * alpha + z1 * beta + z2 * gamma;
                if (z < depth_buffer[i]) {
                
                    // Fill in pixel with correct color.
                    frame_buffer[3 * i] = tri.vert0.vn.x * 255;
                    frame_buffer[3 * i + 1] = tri.vert0.vn.y * 255;
                    frame_buffer[3 * i + 2] = tri.vert0.vn.y * 255;
                    
                    // Update depth buffer.
                    depth_buffer[i] = z;
                }
            }
            new_d12 += d12_x_update;
            new_d20 += d20_x_update;
        }
        d12 += d12_y_update;
  	    d20 += d20_y_update;
    }
    return; 
}
