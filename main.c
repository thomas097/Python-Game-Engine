/* Project ........ Python Game Engine
 * Filename ....... test.c
 * Description .... Test file to try out features.
 * Created by ..... Thomas Bellucci
 * Date ........... Dec 20th, 2020
 * Compile ........ g++ -g -o main main.c rasterization.c utilities.c -lglfw -lGL
 */ 
#include <GLFW/glfw3.h>
#include <iostream>
#include <cstring>
#include <stdlib.h>
#include <vector>
#include "rasterization.h"
#include "utilities.h"
using namespace std;

// Constants
int BUFFER_WIDTH = 640;
int BUFFER_HEIGHT = 480;


int main(void) {
    
    // Initialize the library and create the window.
    if (!glfwInit()) return -1;
    GLFWwindow* window = glfwCreateWindow(BUFFER_WIDTH, BUFFER_HEIGHT, "", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }
    
    // Load Suzanne.
    vector<Tri> model = ImportOBJ("Models/Suzanne2.obj");
    Tri tri;
    
    // Initialize frame and depth buffer.
    unsigned char* frame_buffer = new unsigned char[BUFFER_WIDTH * BUFFER_HEIGHT * 3];
    unsigned char* depth_buffer = new unsigned char[BUFFER_WIDTH * BUFFER_HEIGHT]();


    // Loop until the user closes the window.
    glfwMakeContextCurrent(window);
    while (!glfwWindowShouldClose(window)) {
        // Clear buffers.
        memset(frame_buffer, 0, BUFFER_WIDTH * BUFFER_HEIGHT * 3);
        memset(depth_buffer, 255, BUFFER_WIDTH * BUFFER_HEIGHT);
        glClear(GL_COLOR_BUFFER_BIT);
        
        // Render scene.
        unsigned long size = model.size();
        for (unsigned long i = 0; i < size; i++) {
            tri = model[i];
            RasterizeTriangle(BUFFER_WIDTH, BUFFER_HEIGHT, frame_buffer, depth_buffer, tri);
        }

        // Swap front and back buffers.
        glDrawPixels(BUFFER_WIDTH, BUFFER_HEIGHT, GL_RGB, GL_UNSIGNED_BYTE, frame_buffer);
        glfwSwapBuffers(window);

        // Poll for and process events.
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
