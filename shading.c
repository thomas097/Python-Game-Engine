/* Project ........ Python Game Engine
 * Filename ....... shading.c
 * Description .... Includes all functions used for shading.
 * Created by ..... Thomas Bellucci
 * Date ........... Dec 20th, 2020
 */
#include <Eigen/Dense>
#include <vector>
#include <iostream>
#include <math.h>
#include "types.h"

 
unsigned int texture_lookup(Texture* tex, Eigen::Vector2f* texcoord)
{
    int width = tex->width;
    int height = tex->height;
    
    // Nearest neighbor interpolation.
    int u = floor(width * (*texcoord)(0) + .5);
    int v = floor(height * (1 - (*texcoord)(1)) + .5);
    
    // Repeat texture.
    u = u % width;
    v = v % height;
    if (u < 0) u = width - u;
    if (v < 0) v = width - v;
    
    // Return pixel index.
    return 3 * (v * width + u);
}


void shade_pixel(Eigen::Vector3f* pixel, Eigen::Vector3f* vertex, Eigen::Vector3f* normal, Eigen::Vector2f* texcoord, Texture* texture)
{
    // Look up pixel color from texture.
    unsigned int i = texture_lookup(texture, texcoord);

    /*// Compute lambertian shading from sky (diffuse pass).
    Eigen::Vector3f up_vector(0, 1, 0);
    float luminance = std::max(0.0f, (*normal).dot(up_vector));
    
    // Compute specular highlight from sky lamp.
    Eigen::Vector3f view_vector = -vertex->normalized();
    Eigen::Vector3f h = (view_vector + up_vector).normalized();
    float specular = 100*pow(normal->dot(h), 20);*/

    // Update pixel.
    (*pixel)(0) = texture->data[i];// * luminance + specular;
    (*pixel)(1) = texture->data[i + 1];
    (*pixel)(2) = texture->data[i + 2];
    
    // Clip pixel intensity to 255.
    (*pixel).cwiseMin(255);
}




