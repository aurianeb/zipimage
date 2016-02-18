#include <Imagine/Graphics.h>
#include <algorithm>
#include <iostream>
#include "../quadtree/quadtree.h"
#include "image.h"

using namespace Imagine;
using namespace std;

#ifndef srcPath

#define srcPath(s) ("." s)
#endif

// The path and name of the town file
string default_image_file = srcPath("/running-horse-rect.png");

int main(int argc, char **argv)
{
    // Get image file
    const char *image_file =
    (argc > 1) ? argv[1] : srcPath("/running-horse-rect.png");
    // Load image
    byte* image;
    int width, height;
    cout << "Loading image: " << image_file << endl; 
    loadGreyImage(image_file, image, width, height); //image_file : nom ; image : *byte
    // Print statistics
    cout << "Image size: " << width << "x" << height << endl;
    cout << "Number of pixels: " << width*height << endl;

    /*
    // Display image
    Window window = openWindow(width, height);
    putGreyImage(IntPoint2(0,0), image, width, height);
    // Pause
    click();
    closeWindow(window);
    */


    QuadTree<byte> *A = compression_gris(image, width, height, 50);
    display(A);
    taux_compression(image, width, height);

    byte* tab;
    decompression(A, width, height);

    putGreyImage(IntPoint2(0,0), tab, width, height);

    // Pause
    click();

    // Exit
    return 0;
}
