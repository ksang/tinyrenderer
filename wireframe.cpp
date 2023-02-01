#include "wireframe.h"

void line(Vec2i p0, Vec2i p1, TGAImage &image, TGAColor color) {
    bool steep = false; 
    // If the line is steep ( >45 deg), swap x and y for later looping variable.
    // This will avoid small looping iterations and holes in the line.
    if (std::abs(p0.x-p1.x)<std::abs(p0.y-p1.y)) { 
        std::swap(p0.x, p0.y); 
        std::swap(p1.x, p1.y); 
        steep = true; 
    } 
    if (p0.x>p1.x) { 
        std::swap(p0, p1);
    } 
    int dx = p1.x-p0.x; 
    int dy = p1.y-p0.y; 
    // error here is actually the slope of line, because above code, slope is always < 1.
    // So here is checking if dy > dx, then move y accordingly.
    int derror2 = std::abs(dy)*2; 
    int error2 = 0; 
    int y = p0.y; 
    for (int x=p0.x; x<=p1.x; x++) { 
        if (steep) { 
            image.set(y, x, color); 
        } else { 
            image.set(x, y, color); 
        } 
        error2 += derror2; 
        if (error2 > dx) { 
            y += (p1.y>p0.y?1:-1); 
            error2 -= dx*2; 
        } 
    } 
}

void wireframe(const char* obj_file, TGAImage &image, int width, int height) {
    Model *model = new Model(obj_file);
    for (int i=0; i<model->nfaces(); i++) {
        std::vector<int> face = model->face(i);
        for (int j=0; j<3; j++) {
            Vec3f v0 = model->vert(face[j]);
            Vec3f v1 = model->vert(face[(j+1)%3]);
            int x0 = (v0.x+1.)*width/2.;
            int y0 = (v0.y+1.)*height/2.;
            int x1 = (v1.x+1.)*width/2.;
            int y1 = (v1.y+1.)*height/2.;
            line(Vec2i(x0, y0), Vec2i(x1, y1), image, white);
        }
    }
    delete model;
}