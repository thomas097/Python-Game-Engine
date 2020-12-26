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
#include <stdlib.h>
#include <vector>
#include "rasterization.h"
#include "utilities.h"
#include <chrono>
using namespace std;


// Render settings
const int FRAME_WIDTH = 640;
const int FRAME_HEIGHT = 480;
const int MAX_DRAW_DIST = 1024;


int main(void)
{
    // Load objects, materials and lights.
    Texture tex = load_texture("textures/Scene1_baked.png");
    vector<Tri> mesh = load_mesh("models/Scene1.obj");
    
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
        auto start = std::chrono::high_resolution_clock::now();
        
        rasterize_object(&frame, &mesh, &tex);
        
        auto finish = std::chrono::high_resolution_clock::now();
        int dur = std::chrono::duration_cast<std::chrono::nanoseconds>(finish-start).count();
        printf("fps: %f\n", 1 / (dur* pow(10, -9)));

        // Swap front and back buffers.
        glDrawPixels(FRAME_WIDTH, FRAME_HEIGHT, GL_RGB, GL_UNSIGNED_BYTE, frame.frame_buffer);
        glfwSwapBuffers(window);

        // Poll for and process events.
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
