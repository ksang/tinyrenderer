#ifndef __RASTERIZATION_H__
#define __RASTERIZATION_H__

#include "tgaimage.h"
#include "geometry.h"

void viewport(const int x, const int y, const int w, const int h);
void projection(const double coeff=0); // coeff = -1/c
void lookat(const vec3 eye, const vec3 center, const vec3 up);

struct IShader {
    static TGAColor sample2D(const TGAImage &img, vec2 &uvf) {
        return img.get(uvf[0] * img.width(), uvf[1] * img.height());
    }
    virtual bool fragment(const vec3 bar, TGAColor &color) = 0;
};

void triangle(const vec4 clip_verts[3], IShader &shader, TGAImage &image, std::vector<double> &zbuffer);

void triangle_rasterization(const char* obj_file, TGAImage& image, TGAImage& depth, int width, int height);

#endif //__RASTERIZATION_H__