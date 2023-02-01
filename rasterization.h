#ifndef __RASTERIZATION_H__
#define __RASTERIZATION_H__

#include "geometry.h"
#include "tgaimage.h"



void triangle_rasterization(const char* obj_file, TGAImage& image, int width, int height);

#endif //__RASTERIZATION_H__