#ifndef __WIREFRAME_H__
#define __WIREFRAME_H__
#include <vector>
#include <cmath>
#include "model.h"
#include "geometry.h"
#include "tgaimage.h"

void line(Vec2i p0, Vec2i p1, TGAImage &image, TGAColor color);
void wireframe(const char* obj_file, TGAImage &image, int width, int height);

#endif //__WIREFRAME_H__