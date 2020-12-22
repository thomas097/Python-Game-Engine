#ifndef _UTILITIES_H_
#define _UTILITIES_H_

#include <iostream> 
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <vector> 
#include "types.h"
using namespace std;


int is_quad(const char *string);

vector<Tri> import_OBJ(const char filename[]);

Frame create_frame(int width, int height);

Frame clear_frame(Frame frame, float max_depth);

Object create_object(const char obj_file[], Material mat);

Texture create_texture(const char filename[]);

Vec3D create_Vec3D(int x, int y, int z);

Material create_material(const char texture[], float fresnel);

Light create_light(vector<float> loc, float intensity);

#endif
