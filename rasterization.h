/* Project ........ Python Game Engine
 * Filename ....... rasterization.c
 * Description .... Includes all functions used for rasterization.
 * Created by ..... Thomas Bellucci
 * Date ........... Dec 20th, 2020
 */
#ifndef _RASTERIZATION_H_
#define _RASTERIZATION_H_
#include <math.h>
#include "types.h"
#include <cstdio>

// Computes the implicit line function f for triangle rasterization.
float f(float x0, float y0, float x1, float y1, float x, float y);

// Calculates the minumum of three floats. 
int min3f(float a, float b, float c);

// Calculates the maximum of three floats.
int max3f(float a, float b, float c);

// Rasterizes the triangle spanned by pt0, pt1 and pt2 in buffer.
void RasterizeTriangle(int frame_width, int frame_height, unsigned char* frame_buffer, unsigned char* depth_buffer, Tri tri);

#endif
