#include <vector>
#include <cmath>
#include "tgaimage.h"
#include "wireframe.h"

const int width  = 800;
const int height = 800;

int main(int argc, char** argv) {
	TGAImage image(width, height, TGAImage::RGB);
	
	if (argc >= 2) {
		if (std::string(argv[1]) == "wireframe") {
			if (argc >= 3) {
				wireframe(argv[2], &image, width, height);
			} else {
				wireframe("obj/african_head.obj", &image, width, height);
			}
		}
	}
	image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
	image.write_tga_file("output.tga");
	return 0;
}

