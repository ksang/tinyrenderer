#include <vector>
#include <cmath>
#include "tgaimage.h"
#include "wireframe.h"
#include "rasterization.h"

const int width  = 800;
const int height = 800;

int main(int argc, char** argv) {
	TGAImage image(width, height, TGAImage::RGB);
	TGAImage depth_image(width, height, TGAImage::RGB);
	
	if (argc >= 2) {
		if (std::string(argv[1]) == "wireframe") {
			if (argc >= 3) {
				wireframe(argv[2], image, width, height);
			} else {
				wireframe("obj/african_head/african_head.obj", image, width, height);
			}
		} else if (std::string(argv[1]) == "rasterization") {
			if (argc >= 3) {
				triangle_rasterization(argv[2], image, depth_image, width, height);
			} else {
				triangle_rasterization("obj/african_head/african_head.obj", image, depth_image, width, height);
			}
		}
	}
	image.write_tga_file("output.tga");
	depth_image.write_tga_file("depth.tga");
	return 0;
}

