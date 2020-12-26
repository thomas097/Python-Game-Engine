#ifndef _UTILITIES_H_
#define _UTILITIES_H_

#include <iostream> 
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <vector> 
#include "types.h"
using namespace std;


Mesh load_mesh(const char filename[]);

Texture load_texture(const char filename[]);


#endif
