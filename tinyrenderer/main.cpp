#include <iostream>
#include <fstream>
#include <algorithm>
#include <cmath>
#include "tgaimage.h"
#include "model.h"
#include "geometry.h"
#include <chrono>
#include <thread>
#include <assert.h>
#include <string>

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red   = TGAColor(255, 0,   0,   255);
const TGAColor green = TGAColor(0,	 255, 0,   255);
const TGAColor yellow = TGAColor(255,255, 0,   255);
Model *model = NULL;
const int width = 200;
const int height = 200;

void line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color);
void triangle(Vec2i t0, Vec2i t1, Vec2i t2, TGAImage &image, TGAColor color);

std::ofstream logfile;


// main
int main(int argc, char** argv)
{
	logfile.open("drawlog.txt", std::ofstream::out);

    TGAImage image(width, height, TGAImage::RGB);

	Vec2i t0[3] = {Vec2i(10, 70),   Vec2i(50, 160),   Vec2i(70, 80)};
	Vec2i t1[3] = {Vec2i(180, 50),  Vec2i(150, 1),    Vec2i(70, 180)}; 
	Vec2i t2[3] = {Vec2i(180, 150), Vec2i(120, 160),  Vec2i(130, 180)};
	


	triangle(t0[0], t0[1], t0[2], image, red);
	triangle(t0[0], t0[1],			Vec2i(30,190), 	image, white);
	triangle(t0[0], Vec2i(50,70),	t0[2],			image, green);
	triangle(t1[0], t1[1], t1[2], image, white);
	triangle(t2[0], t2[1], t2[2], image, green);


	image.flip_vertically();
	image.write_tga_file("output.tga");
	
	logfile.close();
	// Drawing wavefront obj model
	// Make sure following const values are set: width = 800, height = 800
	/*
	if (2 == argc)
	{
        model = new Model(argv[1]);
    }
	else
	{
        model = new Model("obj/head.obj");
    }
	
    TGAImage image(width, height, TGAImage::RGB);
   	for(int i = 0; i < model->nfaces(); i++)
	{
		std::vector<int> face = model->face(i);
		for (int j = 0; j < 3; j++)
		{
			Vec3f v0 = model->vert(face[j]);
			Vec3f v1 = model->vert(face[(j+1)%3]);
			int x0 = (v0.x+1.)*width/2.;
			int y0 = (v0.y+1.)*height/2.;
			int x1 = (v1.x+1.)*width/2.;
			int y1 = (v1.y+1.)*height/2.;
			line(x0, y0, x1, y1, image, white);
		}
 	}

	image.flip_vertically();
	image.write_tga_file("output.tga");
	delete model;
	*/
    return 0;
} // end main

// line function
void line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color)
{
	bool steep = false;

	// Transpose the image, in the case the line is steep (change in y is greater than change in
	// x). 
	// At this point, x is really the "long direction" and y is really the "short direction."
	if(std::abs(x0-x1) < std::abs(y0-y1)) 
	{
		std::swap(x0, y0);
		std::swap(x1, y1);
		steep = true;
	} 
	if (x0 > x1) // ensure we draw left to right.
	{
		std::swap(x0, x1);
		std::swap(y0, y1);
	}
	// dx is change between poiint0 to point1, automatically positive or zero, given drawing left
	// to right.
	int dx = x1 - x0;

	// dy, the change between point0 to point1,  can be negative or positive.
	int dy = y1 - y0;
	
	// derror2 is the amount of "error" (in terms of partial steps in the y direction, that haven't
	// been taken) multiplied by 2.
	// There is never a dx error because x will always change my the same or equal amount as y.
	// (because we transposed).
	int derror2 = std::abs(dy) * 2;

	// error2 is the accumulated error multipled by 2, so far.
	int error2 = 0;
	int y = y0;

	// derror2 and error2 represeent error multiplied by 2 because we normally would increment y,
	// whenever error is greater than .5 (half a pixel). By multiplying by 2, we avoid floating
	// point operations.
	
	for (int x = x0; x <= x1; ++x)
	{
		// If not drawing in image boundaries, skip this loop.
		// TODO: this is implemented incorrectly. It should increment x and y appropriately, without
		// 	drawing a pixel.  It shouldn't just skip the loop entirely, because wel'' end up in an
		// 	infinite loop.
		if ( x < 0 || y < 0 || x > width || y > height)
		{
			continue;
		}

		// If transposed, de-transpose before drawing.
		if (steep)
		{
			image.set(y, x, color); 
		}
		else
		{
			image.set(x, y, color);
		}
		
		// Accumulate error (abs(dy)*2)
		error2 += derror2;
		
		// If accumulated error(abs(dy)*2) is greater than dx (guaranteed to be positive difference
		// of movement between the first and second point, along the longer axis), then increment
		// y (along the short movement axis, in the appropriate direction). 
		if (error2 > dx)
		{
			y += (y1 > y0 ? 1 : -1);
			// Reduce error by 2 * dx (amount of error accumlated is equal to one full dx length,
			// multipled by 2 (to avoid floating point math)).
			error2 -= dx * 2;
		}
	}
} // end line function

// line function with Vec2i parameters
void line(Vec2i t0, Vec2i t1, TGAImage &image, TGAColor color)
{
	line(t0.x, t0.y, t1.x, t1.y, image, color);
}

// triangle function
void triangle(Vec2i t0, Vec2i t1, Vec2i t2, TGAImage &image, TGAColor color)
{
	std::cout << "\nDrawing Triangle: ";
	std::cout << "(" + std::to_string(t0.x) + "," + std::to_string(t0.y) + ")";
	std::cout << "(" + std::to_string(t1.x) + "," + std::to_string(t1.y) + ")";
	std::cout << "(" + std::to_string(t2.x) + "," + std::to_string(t2.y) + ")";
	std::cout << "\n";

	line(t0, t1, image, yellow);
	line(t1, t2, image, yellow);
	line(t2, t0, image, yellow);

	// Sort vertices by y-coordinate, descending
	if (t0.y < t1.y) std::swap(t0, t1);
	if (t1.y < t2.y)
	{
		std::swap(t1, t2);
		if (t0.y < t1.y) std::swap(t0, t1);
	}
	assert(t0.y >= t1.y);
	assert(t1.y >= t2.y);
	
	std::cout << "	Sort vertices descending by y: ";
	std::cout << "(" + std::to_string(t0.x) + "," + std::to_string(t0.y) + ")";
	std::cout << "(" + std::to_string(t1.x) + "," + std::to_string(t1.y) + ")";
	std::cout << "(" + std::to_string(t2.x) + "," + std::to_string(t2.y) + ")";
	std::cout << "\n";
	// So, now that t0.y > t1.y > t2.y, we can assume the lines drawn from it, to ther lines will
	// be the left and right boundaries.
	// We also know the second vertex is higher than the last vertex, so the line from the second
	// vertex to the third vertex will be included in the second vertex's "side" for drawing 
	// horizontal lines.
	 
	
	// Initialize x,y position for the line1 line
	int line1_dx = std::abs(t1.x - t0.x);
	int line1_dy = std::abs(t1.y - t0.y);
	int line1_derror2 = std::abs(line1_dx) * 2;
	int line1_error2 = 0;
	int line1_x = t0.x;
	std::cout << "	Line1:";
	std::cout << "(" + std::to_string(t0.x) + "," + std::to_string(t0.y) + ")";
	std::cout << "(" + std::to_string(t1.x) + "," + std::to_string(t1.y) + ")";
	std::cout << "\n";
	std::cout << "		dx: " + std::to_string(line1_dx) + "\n";
	std::cout << "		dy: " + std::to_string(line1_dy) + "\n";
	std::cout << "		derror2: " + std::to_string(line1_derror2) + "\n";
	std::cout << "		x: " + std::to_string(line1_x) + "\n";

	// Initialize x,y position for the line2 line
	int line2_dx = std::abs(t2.x - t0.x);
	int line2_dy = std::abs(t2.y - t0.y);
	int line2_derror2 = std::abs(line2_dx) * 2;
	int line2_error2 = 0;
	int line2_x = t0.x;


	// Initialize x,y position for the line3 line
	int line3_dx = std::abs(t2.x - t1.x);
	int line3_dy = std::abs(t2.y - t1.y);
	int line3_derror2 = std::abs(line3_dx) * 2;
	int line3_error2 = 0;
	int line3_x = t1.x;

	// For each vertical pixel of line1 (from t0 to t1),
	// 		draw a line from that pixel to the vertical pixel of the line line2
	// 		update line1 coordinates
	// 		update line2 coordinates
	for(int y = t0.y; y >= t1.y; y--)
	{
		std::cout << "	Drawing from ";
		std::cout << "(" + std::to_string(line1_x) + "," + std::to_string(y) + ")";
		std::cout << " to ";
		std::cout << "(" + std::to_string(line2_x) + "," + std::to_string(y) + ")";
		std::cout << "\n";

		line(line1_x, y, line2_x, y, image, color);
		
		//Update line1 x coodinate
		line1_error2 += line1_derror2;
		while (line1_error2 > line1_dy)
		{
			line1_x += (t1.x > t0.x ? 1 : -1);
			line1_error2 -= 2 * line1_dy;
		}

		//Update line2 x coordinate
		line2_error2 += line2_derror2;
		while (line2_error2 > line2_dy)
		{
			line2_x += (t2.x > t0.x ? 1 : -1);
			line2_error2 -= 2 * line2_dy;
		}
	}

	// For each vertical pixel of line3 (from t1 to t2)
	// 		draw a line from that pixel to the vertical pixel of the line line2
	// 		update line3 coordinates
	// 		update line2 coordinates
	for(int y = t1.y; y >= t2.y; y--)
	{
		line(line3_x, y, line2_x, y, image, color);
		
		//Update line3 x coordinate
		line3_error2 += line3_derror2;
		while(line3_error2 > line3_dy)
		{
			std::cout << "Entering loop: \n";
			std::cout << "	line3_error2: " + std::to_string(line3_error2) + "\n";
			std::cout << "	line3_dy: " + std::to_string(line3_dy) + "\n";
			std::cout << "Entering other loop: \n";
			line3_x += (t2.x > t1.x ? 1 : -1);
			line3_error2 -= 2 * line3_dy;
			std::cout << "Exiting loop: \n";
			std::cout << "	line3_error2: " + std::to_string(line3_error2) + "\n";
			std::cout << "	line3_dy: " + std::to_string(line3_dy) + "\n";
		}
		//Update line2 x coordinate
		line2_error2 += line2_derror2; 
		while(line2_error2 > line2_dy)
		{
			line2_x += (t2.x > t0.x ? 1 : -1);
			line2_error2 -= 2 * line2_dy;
		}

	}
}

