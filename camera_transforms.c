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
#include "utilities.h"
#include "types.h"


Eigen::Matrix4f camera_transform(Eigen::Vector3f origin, Eigen::Vector3f direction)
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
    Eigen::Matrix4f Mcam = Muvw * Mo;
    return Mcam;
}


Eigen::Matrix4f viewport_transform(unsigned int frame_width, unsigned int frame_height, float fov, float n, float f)
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
    Eigen::Matrix4f Mvp;
    Mvp << nx/2, 0, 0, (nx-1)/2,
           0, ny/2, 0, (ny-1)/2,
           0, 0, 1, 0,
           0, 0, 0, 1;
           
    // Precompute viewport transform.
    return Mvp * Morth * P;
}





// Render settings
const int FRAME_WIDTH = 720;
const int FRAME_HEIGHT = 480;
const int MAX_DRAW_DIST = 1024;

int main(void)
{

    // TODO: Create cube with vertices along edges.
    Eigen::MatrixXf vertices(4, 27);
    int i = 0;
    for (int x = -1; x < 2; x += 1) {
        for (int y = -1; y < 2; y += 1) {
            for (int z = -1; z < 2; z += 1) {
                vertices(0, i) = x;
                vertices(1, i) = y;
                vertices(2, i) = z;
                vertices(3, i) = 1;
                i++;   
            }
        }
    }
    
    // TODO; Create camera 
    Eigen::Vector3f origin(4, 4, 4);
    Eigen::Vector3f direction(-1, -1, -1);
    
    Eigen::Matrix4f Mvp = viewport_transform(FRAME_WIDTH, FRAME_HEIGHT, 30, -1, -50);
    Eigen::Matrix4f Mcam = camera_transform(origin, direction);
    Eigen::Matrix4f M = Mvp * Mcam;
    
    // Transform points.
    Eigen::MatrixXf new_points(3, 27);
    new_points = (M * vertices).colwise().hnormalized();
    
    
    
    
    
    // Initialize the library and create the window.
    if (!glfwInit()) return -1;
    GLFWwindow* window = glfwCreateWindow(FRAME_WIDTH, FRAME_HEIGHT, "", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }
    
    // Initialize frame and depth buffer.
    Frame frame = create_frame(FRAME_WIDTH, FRAME_HEIGHT);
    
    // Loop until the user closes the window.
    glfwMakeContextCurrent(window);
    while (!glfwWindowShouldClose(window)) {
        // Clear buffers.
        clear_frame(frame, MAX_DRAW_DIST);
        glClear(GL_COLOR_BUFFER_BIT);
        
        // Render scene and time execution.
        int x, y;
        unsigned long i;
        for (int j = 0; j < 27; j++ ){
            x = (int) new_points(0, j);
            y = (int) new_points(1, j);
            i = 3 * (FRAME_WIDTH * y + x);
            frame.frame_buffer[i] = 255;
            frame.frame_buffer[i+1] = 255;
            frame.frame_buffer[i+2] = 255;
        }

        // Swap front and back buffers.
        glDrawPixels(FRAME_WIDTH, FRAME_HEIGHT, GL_RGB, GL_UNSIGNED_BYTE, frame.frame_buffer);
        glfwSwapBuffers(window);

        // Poll for and process events.
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}








