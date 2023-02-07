#include "rasterization.h"
#include "wireframe.h"
#include <limits>

mat<4,4> ModelView;
mat<4,4> Viewport;
mat<4,4> Projection;

void viewport(const int x, const int y, const int w, const int h) {
    Viewport = {{{w/2., 0, 0, x+w/2.}, {0, h/2., 0, y+h/2.}, {0,0,1,0}, {0,0,0,1}}};
}

vec3 barycentric(const vec2 tri[3], const vec2 P) {
    mat<3,3> ABC = {{embed<3>(tri[0]), embed<3>(tri[1]), embed<3>(tri[2])}};
    if (ABC.det()<1e-3) return {-1,1,1}; // for a degenerate triangle generate negative coordinates, it will be thrown away by the rasterizator
    return ABC.invert_transpose() * embed<3>(P);
}
 
void triangle(const vec4 clip_verts[3], IShader &shader, TGAImage &image, std::vector<double> &zbuffer) {
    vec4 pts[3]  = { Viewport*clip_verts[0],    Viewport*clip_verts[1],    Viewport*clip_verts[2]    };  // triangle screen coordinates before persp. division
    vec2 pts2[3] = { proj<2>(pts[0]/pts[0][3]), proj<2>(pts[1]/pts[1][3]), proj<2>(pts[2]/pts[2][3]) };  // triangle screen coordinates after  perps. division

    vec2 bboxmin{ std::numeric_limits<double>::max(),  std::numeric_limits<double>::max()};
    vec2 bboxmax{-std::numeric_limits<double>::max(), -std::numeric_limits<double>::max()};
    vec2 clamp{static_cast<double>(image.width()-1), static_cast<double>(image.height()-1)};
    for (int i=0; i<3; i++)
        for (int j=0; j<2; j++) {
            bboxmin[j] = std::max(0.,       std::min(bboxmin[j], pts2[i][j]));
            bboxmax[j] = std::min(clamp[j], std::max(bboxmax[j], pts2[i][j]));
        }
#pragma omp parallel for
    for (int x=(int)bboxmin.x; x<=(int)bboxmax.x; x++) {
        for (int y=(int)bboxmin.y; y<=(int)bboxmax.y; y++) {
            vec3 bc_screen = barycentric(pts2, {static_cast<double>(x), static_cast<double>(y)});
            vec3 bc_clip   = {bc_screen.x/pts[0][3], bc_screen.y/pts[1][3], bc_screen.z/pts[2][3]};
            bc_clip = bc_clip/(bc_clip.x+bc_clip.y+bc_clip.z); // check https://github.com/ssloy/tinyrenderer/wiki/Technical-difficulties-linear-interpolation-with-perspective-deformations
            double frag_depth = vec3{clip_verts[0][2], clip_verts[1][2], clip_verts[2][2]}*bc_clip;
            if (bc_screen.x<0 || bc_screen.y<0 || bc_screen.z<0 || frag_depth > zbuffer[x+y*image.width()]) continue;
            TGAColor color;
            if (shader.fragment(bc_clip, color)) continue; // fragment shader can discard current fragment
            zbuffer[x+y*image.width()] = frag_depth;
            image.set(x, y, color);
        }
    }
}

vec3 light_dir{0,0,-1}; // define light_dir

void orthogonal_triangle(vec3 *pts, std::vector<double> &zbuffer, TGAImage &image, TGAColor color) {
    vec2 bboxmin{std::numeric_limits<double>::max(),  std::numeric_limits<double>::max()};
    vec2 bboxmax{-std::numeric_limits<double>::max(), -std::numeric_limits<double>::max()};
    vec2 clamp{static_cast<double>(image.width()-1), static_cast<double>(image.height()-1)};
    for (int i=0; i<3; i++) {
        for (int j=0; j<2; j++) {
            bboxmin[j] = std::max(0.,      std::min(bboxmin[j], pts[i][j]));
            bboxmax[j] = std::min(clamp[j], std::max(bboxmax[j], pts[i][j]));
        }
    }
    for (int x=int(bboxmin.x); x<=int(bboxmax.x); x++) {
        for (int y=bboxmin.y; y<=int(bboxmax.y); y++) {
            //vec2 pts2[3] = {{pts[0].x, pts[0].y}, {pts[1].x, pts[1].y}, {pts[2].x, pts[2].y}}; 
            vec2 pts2[3] = { proj<2>(pts[0]), proj<2>(pts[1]), proj<2>(pts[2]) }; 
            vec3 bc_screen  = barycentric(pts2, {static_cast<double>(x), static_cast<double>(y)});
            if (bc_screen.x<0 || bc_screen.y<0 || bc_screen.z<0) continue;
            double z = 0;
            for (int i=0; i<3; i++) z += pts[i][2]*bc_screen[i];
            if (zbuffer[int(x+y*image.width())] < z) {
                zbuffer[int(x+y*image.width())] = z;
                image.set(x, y, color);
            }
            // image.set(P.x, P.y, color);
        }
    }
}

void write_depth( TGAImage &depth, std::vector<double> &zbuffer, int width, int height){
    // Write depth
    for (int i=0; i< width; i++) {
        for (int j=0; j< height; j++){
            double d = zbuffer[i+j*width];
            depth.set(i, j, TGAColor{{std::uint8_t(d*255), std::uint8_t(d*255), std::uint8_t(d*255), 255}});
        }
    }
}

void triangle_rasterization(const char* obj_file, TGAImage &image, TGAImage &depth, int width, int height) {
    Model *model = new Model(obj_file);
    std::vector<double> zbuffer(width*height, std::numeric_limits<double>::min());
    for (int i=0; i<model->nfaces(); i++) {
        vec3 screen_coords[3];
        vec3 world_coords[3]; 
        for (int j=0; j<3; j++) { 
            vec3 v = model->vert(i, j); 
            screen_coords[j] = vec3{(v.x+1.)*width/2.+.5, (v.y+1.)*height/2.+.5, v.z};
            world_coords[j] = v;
        }
        vec3 n = cross(world_coords[2]-world_coords[0], world_coords[1]-world_coords[0]);
        n = n.normalized();
        
        float intensity = n*light_dir;
        if(intensity > 0) {
            orthogonal_triangle(screen_coords, zbuffer, image, TGAColor{{std::uint8_t(intensity*255), std::uint8_t(intensity*255), std::uint8_t(intensity*255), 255}}); 
        }
    }
    write_depth(depth, zbuffer, width, height);
    delete model;
}