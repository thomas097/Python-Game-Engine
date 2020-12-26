#ifndef _UTILITIES_H_
#define _UTILITIES_H_

#include <iostream> 
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <vector> 
#include "types.h"
using namespace std;


int is_tri(const char *string);

vector<Tri> load_mesh(const char filename[]);

Texture load_texture(const char filename[]);


Frame create_frame(int width, int height);

Frame clear_frame(Frame frame, float max_depth);

#endif
