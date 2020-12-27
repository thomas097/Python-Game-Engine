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


int is_backface(Eigen::Vector3f normal) {
    return normal(2) < 0;
}


unsigned int texture_lookup(Texture* tex, double s, double t) {
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


double f(double x0, double y0, double x1, double y1, double x, double y) {
    return (y0 - y1) * x + (x1 - x0) * y + (x0 * y1) - (x1 * y0);
}


void rasterize_mesh_triangle(Camera* cam, Eigen::MatrixXf* v, Eigen::MatrixXf* vn, Eigen::MatrixXf* vt, Tri tri, Texture* tex) 
{   
    // Compute normal vector for triangle.
    Eigen::Vector4f normal0 = vn->col(tri.ivn0);
    Eigen::Vector4f normal1 = vn->col(tri.ivn1);
    Eigen::Vector4f normal2 = vn->col(tri.ivn2);
    Eigen::Vector3f normal = (normal0 + normal1 + normal2).hnormalized().normalized();
    
    // Backface culling
    if (is_backface(normal)) return;
    
    // Unpack vertex coordinates.
    double x0 = (*v).col(tri.iv0)(0);
    double y0 = (*v).col(tri.iv0)(1);
    double z0 = (*v).col(tri.iv0)(2);
    
    double x1 = (*v).col(tri.iv1)(0);
    double y1 = (*v).col(tri.iv1)(1);
    double z1 = (*v).col(tri.iv1)(2);
    
    double x2 = (*v).col(tri.iv2)(0);
    double y2 = (*v).col(tri.iv2)(1);
    double z2 = (*v).col(tri.iv2)(2);
    
    // Unpack texture coordinates.
    double s0 = (*vt).col(tri.ivt0)(0);
    double t0 = (*vt).col(tri.ivt0)(1);

    double s1 = (*vt).col(tri.ivt1)(0);
    double t1 = (*vt).col(tri.ivt1)(1);
    
    double s2 = (*vt).col(tri.ivt2)(0);
    double t2 = (*vt).col(tri.ivt2)(1);
     
    // Determine bounding box for triangle.
    int x_min = floor(min(x0, min(x1, x2)));
    int x_max = ceil(max(x0, max(x1, x2)));
    int y_min = floor(min(y0, min(y1, y2)));
    int y_max = ceil(max(y0, max(y1, y2)));
    
    // Pre-compute f values.
    double fa = 1 / f(x1, y1, x2, y2, x0, y0);
    double fb = 1 / f(x2, y2, x0, y0, x1, y1);
    double fg = 1 / f(x0, y0, x1, y1, x2, y2);
    
    // Precompute whether fa and f for offscreen point are the same.
    double fa_off = f(x1, y1, x2, y2, -1, -1) / fa > 0;
    double fb_off = f(x2, y2, x0, y0, -1, -1) / fb > 0;
    double fg_off = f(x0, y0, x1, y1, -1, -1) / fg > 0;
    
    // Update alpha and beta incrementally.
    double alpha_init = f(x1, y1, x2, y2, x_min, y_min) * fa;
    double beta_init = f(x2, y2, x0, y0, x_min, y_min) * fb;
    double alpha_x_update = (y1 - y2) * fa; 
    double beta_x_update = (y2 - y0) * fb;
    double alpha_y_update = (x2 - x1) * fa;
    double beta_y_update = (x0 - x2) * fb;
    
    // Frequently accessed variables.
    int frame_width = cam->frame_width;
    int frame_height = cam->frame_height;
    unsigned char* frame_buffer = cam->frame_buffer;
    double* depth_buffer = cam->depth_buffer;
    uint8_t* texture = tex->data;
    double min_draw_dist = cam->min_draw_dist;
    
    signed int y, x;
    unsigned int j, i;
    double alpha, beta, gamma, z, s, t;
    
    for (y = y_min; y <= y_max; y++) {
    
        // Update barycentric coordinates.
        alpha = alpha_init;
      	beta = beta_init;
      	gamma = 1 - (alpha + beta);
      	
        for (x = x_min; x <= x_max; x++) {
        
            // Boundary test: Check whether (y, x) lies withing frame bounds.
            if (x > -1 && y > -1 && x < frame_width && y < frame_height) {
               
                // Triangle test: Check whether (y, x) is included in triangle.
                if (alpha >= 0 && beta >= 0 && gamma >= 0) {
                    if ((alpha > 0 || fa_off) && (beta > 0 || fb_off) && (gamma > 0 || fg_off)) {        
                               
                        // Compute vertex depth (more negative is further away).
                        z = z0 * alpha + z1 * beta + z2 * gamma;
                     
                        // Depth test.
                        i = frame_width * (frame_height - y) + x; 
                        if (z > depth_buffer[i] && z < min_draw_dist) {
                        
                            // Compute texture coordinate.
                            s = alpha * s0 + beta * s1 + gamma * s2;
                            t = alpha * t0 + beta * t1 + gamma * t2;
                            
                            // Fill in pixel with correct color.
                            j = texture_lookup(tex, s, t);
                            frame_buffer[3 * i] = texture[j];
                            frame_buffer[3 * i + 1] = texture[j + 1];
                            frame_buffer[3 * i + 2] = texture[j + 2];
                            
                            // Update depth buffer.
                            depth_buffer[i] = z;
                        }
                    }
                }
            }
            // Update barycentric coords.
            alpha += alpha_x_update;
            beta += beta_x_update;
            gamma = 1 - (alpha + beta);
        }
        alpha_init += alpha_y_update;
  	    beta_init += beta_y_update;   
    }
    return; 
}


void rasterize_mesh(Camera* cam, Mesh* mesh, Texture* tex)
{  
    // Transform vertices in world coordinates into camera coordinates.
    Eigen::MatrixXf M, v, vn;
    M = (*cam->Mvp) * (*cam->Mcam);
    v = (M * (*mesh->v)).colwise().hnormalized();
    
    // Transform normals.
    vn = (*cam->Mcam) * (*mesh->vn);

    vector<Tri> faces = (*mesh->f);
    unsigned long num_faces = mesh->f->size();    
    for (unsigned long i = 0; i < num_faces; i++) {
        rasterize_mesh_triangle(cam, &v, &vn, mesh->vt, faces[i], tex);
    }
}






