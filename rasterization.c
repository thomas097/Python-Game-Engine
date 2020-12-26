/* Project ........ Python Game Engine
 * Filename ....... rasterization.c
 * Description .... Includes all functions used for rasterization.
 * Created by ..... Thomas Bellucci
 * Date ........... Dec 20th, 2020
 */  
#include <math.h>
#include <cstdio>
#include <iostream>
#include <algorithm>
#include <vector>
#include <Eigen/Dense>
#include <Eigen/Core>
#include "types.h"
using namespace std;


#define NUM_THREADS 4


int is_backface(Eigen::Vector3f normal) {
    return normal(2) > 0;
}


unsigned int texture_lookup(Texture* tex, float s, float t) {
    int width = tex->width;
    int height = tex->height;
    
    // Nearest neighbor interpolation.
    int u = floor(width * s + .5);
    int v = floor(height * (1 - t) + .5);
    
    // Repeat texture.
    u = u % width;
    v = v % height;
    if (u < 0) u = width - u;
    if (v < 0) v = width - v;
    
    // Return pixel index.
    return 3 * (v * width + u);
}


float f(float x0, float y0, float x1, float y1, float x, float y) {
    return (y0 - y1) * x + (x1 - x0) * y + x0 * y1 - x1 * y0;
}


void rasterize_mesh_triangle(Camera* cam, Eigen::MatrixXf* v, Eigen::MatrixXf* vn, Eigen::MatrixXf* vt, Tri tri, Texture* tex) 
{   
    // Compute normal vector.
    //Eigen::Vector4f normal0 = vn->col(tri.ivn0);
    //Eigen::Vector4f normal1 = vn->col(tri.ivn1);
    //Eigen::Vector4f normal2 = vn->col(tri.ivn2);
    //Eigen::Vector3f normal = (normal0 + normal1 + normal2).hnormalized().normalized();
    
    // Backface culling
    //if (is_backface(normal)) return;
    
    // Unpack vertex coordinates.
    float x0 = (*v).col(tri.iv0)(0);
    float y0 = (*v).col(tri.iv0)(1);
    float z0 = (*v).col(tri.iv0)(2);
    
    float x1 = (*v).col(tri.iv1)(0);
    float y1 = (*v).col(tri.iv1)(1);
    float z1 = (*v).col(tri.iv1)(2);
    
    float x2 = (*v).col(tri.iv2)(0);
    float y2 = (*v).col(tri.iv2)(1);
    float z2 = (*v).col(tri.iv2)(2);
    
    // Unpack texture coordinates.
    float s0 = (*vt).col(tri.ivt0)(0);
    float t0 = (*vt).col(tri.ivt0)(1);

    float s1 = (*vt).col(tri.ivt1)(0);
    float t1 = (*vt).col(tri.ivt1)(1);
    
    float s2 = (*vt).col(tri.ivt2)(0);
    float t2 = (*vt).col(tri.ivt2)(1);
     
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
    
    int frame_width = cam->frame_width;
    int frame_height = cam->frame_height;
    
    int y, x, in_triangle, i;
    unsigned int j;
    float z, s, t;
    
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
                    i = frame_width * (frame_height - y) + x; 
                    if (z < cam->depth_buffer[i]) {
                    
                        // Compute texture coordinate.
                        s = alpha * s0 + beta * s1 + gamma * s2;
                        t = alpha * t0 + beta * t1 + gamma * t2;
                        
                        // Fill in pixel with correct color.
                        j = texture_lookup(tex, s, t);
                        cam->frame_buffer[3 * i] = tex->data[j];
                        cam->frame_buffer[3 * i + 1] = tex->data[j + 1];
                        cam->frame_buffer[3 * i + 2] = tex->data[j + 2];
                        
                        // Update depth buffer.
                        cam->depth_buffer[i] = z;
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


void rasterize_mesh(Camera* cam, Mesh* mesh, Texture* tex)
{  
    // Transform word coordinates into camera coordinates.
    Eigen::MatrixXf M, v;
    M = (*cam->Mvp) * (*cam->Mcam);
    v = (M * (*mesh->v)).colwise().hnormalized();

    vector<Tri> faces = (*mesh->f);
    unsigned long num_faces = mesh->f->size();    
    for (unsigned long i = 0; i < num_faces; i++) {
        rasterize_mesh_triangle(cam, &v, mesh->vn, mesh->vt, faces[i], tex);
    }
}






