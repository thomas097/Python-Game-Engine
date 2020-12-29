/* Project ........ Python Game Engine
 * Filename ....... test.c
 * Description .... Test file to try out features.
 * Created by ..... Thomas Bellucci
 * Date ........... Dec 20th, 2020
 * Compile ........ g++ -O3 -g -o main main.c shading.c rasterization.c camera.c imports.c -lglfw -lGL
 */ 
#include <GLFW/glfw3.h>
#include <iostream>
#include <cstring>
#include <math.h>
#include <stdlib.h>
#include <chrono>
#include "imports.h"
#include "camera.h"
#include "rasterization.h"
using namespace std;


// Render settings
const int FRAME_WIDTH = 640;
const int FRAME_HEIGHT = 480;
const float FOV = 40;
const float MIN_DRAW_DIST = 0.001f;
const float MAX_DRAW_DIST = 100.0f;



int main(void)
{
    // Load object and its texture.
    Object obj = load_object("models/Scene2.obj",
                             "textures/Scene2_baked.png");
    
    // Create a camera  
    Eigen::Vector3f origin;
    origin << 5, -5, 5;
    
    Eigen::Vector3f direction;
    direction = -origin.normalized();
    
    Camera cam = create_camera(FRAME_WIDTH, FRAME_HEIGHT, origin, direction, FOV, MIN_DRAW_DIST, MAX_DRAW_DIST);
    
    // Define rotation matrix
    float theta = 0.02f;
    Eigen::Matrix3f R;
    R << cos(theta), -sin(theta), 0,
         sin(theta), cos(theta), 0,
         0, 0, 1;
    
    // Initialize the library and create the window.
    if (!glfwInit()) return -1;
    GLFWwindow* window = glfwCreateWindow(FRAME_WIDTH, FRAME_HEIGHT, "", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    // Loop until the user closes the window.
    glfwMakeContextCurrent(window);
    while (!glfwWindowShouldClose(window)) {
        // Clear buffers.
        reset_camera(cam);
        glClear(GL_COLOR_BUFFER_BIT);
        
        // Render scene and time execution.
        auto start = std::chrono::high_resolution_clock::now();
        
        // Render scene
        origin = R * origin;
        direction = -origin.normalized();
        move_camera(&cam, origin, direction);
        rasterize_mesh(&cam, &obj);
        
        auto finish = std::chrono::high_resolution_clock::now();
        int dur = std::chrono::duration_cast<std::chrono::nanoseconds>(finish-start).count();
        printf("fps: %f\n", 1 / (dur* pow(10, -9)));

        // Swap front and back buffers.
        glDrawPixels(FRAME_WIDTH, FRAME_HEIGHT, GL_RGB, GL_UNSIGNED_BYTE, cam.frame_buffer);
        glfwSwapBuffers(window);

        // Poll for and process events.
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
