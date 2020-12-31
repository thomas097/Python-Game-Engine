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
#include "shading.h"
using namespace std;


int is_backface(Eigen::Vector3f normal) {
    return normal(2) < 0;
}


double f(Eigen::Vector3f v0, Eigen::Vector3f v1, double x, double y) {
    return (v0(1) - v1(1)) * x + (v1(0) - v0(0)) * y + (v0(0) * v1(1)) - (v1(0) * v0(1));
}


void rasterize_mesh_triangle(Camera* cam, Eigen::MatrixXf* v, Eigen::MatrixXf* vn, Eigen::MatrixXf* vt, Tri tri, Texture* texture) 
{   
    // Compute normal vector for triangle.
    Eigen::Vector3f vn0 = vn->col(tri.ivn0).head<3>();
    Eigen::Vector3f vn1 = vn->col(tri.ivn1).head<3>();
    Eigen::Vector3f vn2 = vn->col(tri.ivn2).head<3>();
    Eigen::Vector3f normal = (vn0 + vn1 + vn2).normalized();
    
    // Backface culling
    if (is_backface(normal)) return;

    // Unpack vertex coordinates.
    Eigen::Vector3f v0 = v->col(tri.iv0).head<3>();
    Eigen::Vector3f v1 = v->col(tri.iv1).head<3>();
    Eigen::Vector3f v2 = v->col(tri.iv2).head<3>();
    
    // Unpack texture coordinates.
    Eigen::Vector2f vt0 = vt->col(tri.ivt0);
    Eigen::Vector2f vt1 = vt->col(tri.ivt1);
    Eigen::Vector2f vt2 = vt->col(tri.ivt2);
     
    // Determine bounding box for triangle.
    int x_min = floor(min(v0(0), min(v1(0), v2(0))));
    int y_min = floor(min(v0(1), min(v1(1), v2(1))));
    int x_max = ceil(max(v0(0), max(v1(0), v2(0))));
    int y_max = ceil(max(v0(1), max(v1(1), v2(1))));
    
    // Pre-compute f values.
    double fa = 1 / f(v1, v2, v0(0), v0(1));
    double fb = 1 / f(v2, v0, v1(0), v1(1));
    double fg = 1 / f(v0, v1, v2(0), v2(1));
    
    // Precompute whether fa and f for offscreen point are the same.
    double fa_off = f(v1, v2, -1, -1) / fa > 0;
    double fb_off = f(v2, v0, -1, -1) / fb > 0;
    double fg_off = f(v0, v1, -1, -1) / fg > 0;
    
    // Update alpha and beta incrementally.
    double alpha_init = f(v1, v2, x_min, y_min) * fa;
    double beta_init = f(v2, v0, x_min, y_min) * fb;
    double alpha_x_update = (v1(1) - v2(1)) * fa;
    double alpha_y_update = (v2(0) - v1(0)) * fa; 
    double beta_x_update = (v2(1) - v0(1)) * fb;
    double beta_y_update = (v0(0) - v2(0)) * fb;
    
    // Frequently accessed variables.
    int frame_width = cam->frame_width;
    int frame_height = cam->frame_height;
    unsigned char* frame_buffer = cam->frame_buffer;
    double* depth_buffer = cam->depth_buffer;
    uint8_t* texture_data = texture->data;
    double min_draw_dist = cam->min_draw_dist;
    
    signed int y, x;
    unsigned int i, j;
    double alpha, beta, gamma, z, s, t;
    Eigen::Vector3f vertex, pixel;
    Eigen::Vector2f texcoord;
    
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
                               
                        // Interpolate vertex position.
                        vertex = alpha * v0 + beta * v1 + gamma * v2;
                     
                        // Depth test.
                        i = frame_width * (frame_height - y) + x; 
                        if (vertex(2) > depth_buffer[i] && vertex(2) < 0) {
                        
                            // Interpolate texture coordinate.
                            texcoord = alpha * vt0 + beta * vt1 + gamma * vt2;
                            
                            // Interpolate normals.
                            normal = alpha * vn0 + beta * vn1 + gamma * vn2;
                            
                            // Fill in pixel with correct color.
                            shade_pixel(&pixel, &vertex, &normal, &texcoord, texture);
                            frame_buffer[3 * i] = pixel(0);
                            frame_buffer[3 * i + 1] = pixel(1);
                            frame_buffer[3 * i + 2] = pixel(2);
                            
                            // Update depth buffer.
                            depth_buffer[i] = vertex(2);
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


void rasterize_mesh(Camera* cam, Object* obj)
{  
    // Transform vertices in world coordinates into camera coordinates.
    Mesh* mesh = obj->mesh;
    Eigen::MatrixXf M, v, vn;
    M = (*cam->Mvp) * (*cam->Mcam);
    v = (M * (*mesh->v)).colwise().hnormalized();
    
    // Transform normals.
    Eigen::MatrixXf M_inv_T = M.inverse().transpose();
    vn = (M_inv_T * (*obj->mesh->vn)).colwise().hnormalized();
    
    // Unpack texture
    Texture* texture = obj->texture;

    vector<Tri> faces = (*mesh->f);
    unsigned long num_faces = faces.size();    
    for (unsigned long i = 0; i < num_faces; i++) {
        rasterize_mesh_triangle(cam, &v, &vn, mesh->vt, faces[i], texture);
    }
}






