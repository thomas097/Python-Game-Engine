/* Project ........ Python Game Engine
 * Filename ....... shading.c
 * Description .... Includes all functions used for rasterization.
 * Created by ..... Thomas Bellucci
 * Date ........... Dec 20th, 2020
 */  
#include <math.h>
#include <cstdio>
#include <algorithm>
#include <vector>
#include "types.h"
#include "utilities.h"
#include "vmath.h"
using namespace std;


Vec3D texture_lookup(Texture tex, float s, float t) {
    int width = tex.width;
    int height = tex.height;
    
    // Nearest neighbor interpolation.
    int u = floor(width * s + .5);
    int v = floor(height * (1 - t) + .5);
    
    // Repeat texture.
    u = u % width;
    v = v % height;
    if (u < 0) width - u;
    if (v < 0) width - v;
    
    // Put pixel in vector.
    Vec3D pixel;
    int i = 3 * (v * width + u);
    pixel.x = tex.data[i];
    pixel.y = tex.data[i + 1];
    pixel.z = tex.data[i + 2];
    return pixel;
}



Vec3D shade_vertex(Vertex vertex, std::vector<Light> lights, Material mat) {
    float min_value = 0.0f; 
    Vec3D color = texture_lookup(mat.tex, vertex.vt.s, vertex.vt.t);
    Vec3D normal = normalizeV3(vertex.vn);
    Vec3D view = normalizeV3(vertex.v);
    Vec3D L_i;
    
    // Compute diffuse shading.
    float illum = 0.0f;
    for (int i = 0; i < lights.size(); i++) {
        L_i = normalizeV3(subtractV3(lights[i].loc, vertex.v));
        illum += lights[i].intensity * max(min_value, dot_prodV3(normal, L_i));
    }
    color = scaleV3(color, illum);
    
    // Compute fresnel.
    float f = 255 * (1 - abs(dot_prodV3(normal, view)));
    color = add_scalarV3(color, mat.fresnel * f);
    
    // Clip shader values.
    return clipV3(color, 255);
}
