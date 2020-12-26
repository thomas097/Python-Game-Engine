/* Project ........ Python Game Engine
 * Filename ....... test.c
 * Description .... Test file to try out features.
 * Created by ..... Thomas Bellucci
 * Date ........... Dec 20th, 2020
 * Compile ........ g++ -O3 -g -o main main.c rasterization.c shading.c utilities.c vmath.c -lglfw -lGL
 */ 
#include <GLFW/glfw3.h>
#include <iostream>
#include <cstring>
#include <math.h>
#include <stdlib.h>
#include <chrono>
#include "imports.h"
#include "camera_transforms.h"
#include "rasterization.h"
using namespace std;


// Render settings
const int FRAME_WIDTH = 640;
const int FRAME_HEIGHT = 480;
const int MAX_DRAW_DIST = 1024;



Camera create_camera(int frame_width, int frame_height, Eigen::Vector3f origin, Eigen::Vector3f direction, float fov, float clip_start, float clip_end, float max_draw_dist)
{
    // Make local copy of origin and direction.
    Eigen::Vector3f* origin_cpy = new Eigen::Vector3f;
    (*origin_cpy) = origin;
    Eigen::Vector3f* direction_cpy = new Eigen::Vector3f;
    (*direction_cpy) = direction;
    
    Camera cam;
    cam.Mvp = viewport_transform(frame_width, frame_height, fov, -clip_start, -clip_end);
    cam.Mcam = camera_transform(origin, direction);
    cam.origin = origin_cpy;
    cam.direction = direction_cpy;
    cam.frame_width = frame_width;
    cam.frame_height = frame_height;
    cam.frame_buffer = new unsigned char[frame_width * frame_height * 3];
    cam.depth_buffer = new float[frame_width * frame_height];
    cam.max_draw_dist = max_draw_dist;
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


int main(void)
{
    // Load object and its texture.
    Texture tex = load_texture("textures/Scene2_baked.png");
    Mesh mesh = load_mesh("models/Scene2.obj");
    
    // Create a camera
    Eigen::Vector3f origin(5, 5, 5);
    Eigen::Vector3f direction(-0.5774, -0.5774, -0.5774);
    Camera cam = create_camera(FRAME_WIDTH, FRAME_HEIGHT, origin, direction, 30, 0.01f, 15.0f, MAX_DRAW_DIST);
    
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
        origin(0) = origin(1) = origin(2) = origin(0) + 0.01;
        move_camera(&cam, origin, direction);
        rasterize_mesh(&cam, &mesh, &tex);
        
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
