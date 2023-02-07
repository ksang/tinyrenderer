#ifndef __WIREFRAME_H__
#define __WIREFRAME_H__
#include <vector>
#include <cmath>
#include "model.h"
#include "geometry.h"
#include "tgaimage.h"

void line(vec2 p0, vec2 p1, TGAImage &image, TGAColor color);
void wireframe(const char* obj_file, TGAImage &image, int width, int height);

#endif //__WIREFRAME_H__