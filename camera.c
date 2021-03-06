/* Project ........ Python Game Engine
 * Filename ....... camera_transforms.c
 * Description .... Functions for camera projection.
 * Created by ..... Thomas Bellucci
 * Date ........... Dec 20th, 2020
 */  
#include <GLFW/glfw3.h>
#include <math.h>
#include <Eigen/Dense>
#include <iostream>
#include "imports.h"
#include "types.h"



Eigen::Matrix4f* camera_transform(Eigen::Vector3f origin, Eigen::Vector3f direction)
{
    // Define orthogonal camera basis (u, v, w).
    Eigen::Vector3f t(0, 0, 1);                      // up-vector (assuming z = up)
    Eigen::Vector3f w = -direction.normalized();
    Eigen::Vector3f u = (t.cross(w)).normalized();
    Eigen::Vector3f v = w.cross(u);
    
    // Transform world points to camera coordinates.
    Eigen::Matrix4f Muvw;
    Muvw << u(0), u(1), u(2), 0,
            v(0), v(1), v(2), 0,
            w(0), w(1), w(2), 0,
            0, 0, 0, 1;
            
    // Transform world points to camera origin.
    Eigen::Matrix4f Mo;
    Mo << 1, 0, 0, -origin(0),
          0, 1, 0, -origin(1),
          0, 0, 1, -origin(2),
          0, 0, 0, 1;
      
    // Transform world points into camera coordinates.
    Eigen::Matrix4f* Mcam = new Eigen::Matrix4f(4, 4);
    (*Mcam) = Muvw * Mo;
    return Mcam;
}


Eigen::Matrix4f* viewport_transform(unsigned int frame_width, unsigned int frame_height, float fov, float n, float f)
{
    // Compute t, half the height of the frustum.
    float t = n * tan(M_PI * fov / 360);
    
    // Knowing frame width/height ratio, compute r.
    float nx = (float) frame_width;
    float ny = (float) frame_height;
    float r = t * (nx / ny);
    
    // Perspective projection matrix.
    Eigen::Matrix4f P;
    P << n, 0, 0, 0,
         0, n, 0, 0,
         0, 0, n+f, -n*f,
         0, 0, 1, 0;
         
    // Orthographic projection (assuming l=-r & b=-t).
    Eigen::Matrix4f Morth;
    Morth << 1/r, 0, 0, 0,
             0, 1/t, 0, 0,
             0, 0, 2/(n-f), -(n+f)/(n-f),
             0, 0, 0, 1;
           

    // Canonical volume transform.
    Eigen::Matrix4f Mview;
    Mview << nx/2, 0, 0, (nx-1)/2,
             0, ny/2, 0, (ny-1)/2,
             0, 0, 1, 0,
             0, 0, 0, 1;
             
    // Precompute viewport transform.
    Eigen::Matrix4f* Mvp = new Eigen::Matrix4f(4, 4);
    (*Mvp) = Mview * Morth * P;
    return Mvp;
}


Camera create_camera(int frame_width, int frame_height, Eigen::Vector3f origin, Eigen::Vector3f direction, float fov, float min_draw_dist, float max_draw_dist)
{
    // Make local copy of origin and direction.
    Eigen::Vector3f* origin_cpy = new Eigen::Vector3f;
    (*origin_cpy) = origin;
    Eigen::Vector3f* direction_cpy = new Eigen::Vector3f;
    (*direction_cpy) = direction;
    
    Camera cam;
    cam.Mvp = viewport_transform(frame_width, frame_height, fov, -min_draw_dist, -max_draw_dist);
    cam.Mcam = camera_transform((*origin_cpy), (*direction_cpy));
    cam.origin = origin_cpy;
    cam.direction = direction_cpy;
    cam.frame_width = frame_width;
    cam.frame_height = frame_height;
    cam.frame_buffer = new unsigned char[frame_width * frame_height * 3];
    cam.depth_buffer = new double[frame_width * frame_height];
    cam.min_draw_dist = -min_draw_dist;
    cam.max_draw_dist = -max_draw_dist;
    return cam;
}


void move_camera(Camera* cam, Eigen::Vector3f origin, Eigen::Vector3f direction)
{
    // Make local copy of origin and direction.
    Eigen::Vector3f* origin_cpy = new Eigen::Vector3f;
    (*origin_cpy) = origin;
    Eigen::Vector3f* direction_cpy = new Eigen::Vector3f;
    (*direction_cpy) = direction;
    
    cam->Mcam = camera_transform(origin, direction);
    cam->origin = origin_cpy;
    cam->direction = direction_cpy;
    return;
}


Camera reset_camera(Camera cam)
{
    unsigned long size = cam.frame_width * cam.frame_height;
    memset(cam.frame_buffer, 0, size * 3);
    for (unsigned long i = 0; i < size; i++){
        cam.depth_buffer[i] = cam.max_draw_dist;
    }
    return cam;
}






