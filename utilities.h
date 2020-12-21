/* Project ........ Python Game Engine
 * Filename ....... utilities.c
 * Description .... Defines various utility functions, incl. I/O functions.
 * Created by ..... Thomas Bellucci
 * Date ........... Dec 20th, 2020
 */ 
#ifndef _UTILITIES_H_
#define _UTILITIES_H_

#include <iostream> 
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <vector> 
#include "types.h"
using namespace std;


// Returns True when string starts with prefix.
int StartsWith(const char *string, const char *prefix);


vector<Tri> ImportOBJ(const char filename[]);

#endif
