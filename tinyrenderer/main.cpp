#include <iostream>
#include <fstream>
#include <algorithm>
#include <cmath>
#include "tgaimage.h"
#include "model.h"
#include "geometry.h"
#include <chrono>
#include <thread>

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
	if(std::abs(x0-x1) < std::abs(y0-y1)) // if the line is steep, transpose the image
	{
		std::swap(x0, y0);
		std::swap(x1, y1);
		steep = true;
	} 
	if (x0 > x1) // make it left-to-right
	{
		std::swap(x0, x1);
		std::swap(y0, y1);
	}
	int dx = x1 - x0;
	int dy = y1 - y0;
	int derror2 = std::abs(dy) * 2;
	int error2 = 0;
	int y = y0;
	for (int x = x0; x <= x1; ++x)
	{
		// Check if drawing in image boundaries
		if ( x < 0 || y < 0 || x > width || y > height)
		{
			continue;
		}
		if (steep)
		{
			image.set(y, x, color); // if transposed, de-transpose
		}
		else
		{
			image.set(x, y, color);
		}
		error2 += derror2;
		if (error2 > dx)
		{
			y += (y1 > y0 ? 1 : -1);
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

	// So, now that t0.y > t1.y > t2.y, we can assume the lines drawn from it, to ther lines will
	// be the left and right boundaries.
	// We also know the second vertex is higher than the last vertex, so the line from the second
	// vertex to the third vertex will be included in the second vertex's "side" for drawing horizontal lines.
	 
	
	// Initialize x,y position for the line1 line, line2 line, and line3 line
	int line1_dx = std::abs(t1.x - t0.x);
	int line1_dy = std::abs(t1.y - t0.y);
	int line1_derror2 = std::abs(line1_dx) * 2;
	int line1_error2 = 0;
	int line1_x = t0.x;

	int line2_dx = std::abs(t2.x - t0.x);
	int line2_dy = std::abs(t2.y - t0.y);
	int line2_derror2 = std::abs(line2_dx) * 2;
	int line2_error2 = 0;
	int line2_x = t0.x;

	int line3_dx = std::abs(t2.x - t1.x);
	int line3_dy = std::abs(t2.y - t2.x);
	int line3_derror2 = std::abs(line3_dx) * 2;
	int line3_error2 = 0;
	int line3_x = t1.x;

	// For each vertical pixel of line1 (from t0 to t1),
	// 		draw a line from that pixel to the vertical pixel of the line line2
	// 		update line1 coordinates
	// 		update line2 coordinates
	for(int y = t0.y; y >= t1.y; y--)
	{
		
		line(line1_x, y, line2_x, y, image, color);
		
		//Update line1 x coodinate
		line1_error2 += line1_derror2;
		while (line1_error2 > line1_dy)
		{
			line1_x += (t1.x > t0.x ? 1 : -1);
			line1_error2 -= 2 * line1_dx;
		}

		//Update line2 x coordinate
		line2_error2 += line2_derror2;
		while (line2_error2 > line2_dy)
		{
			line2_x += (t2.x > t0.x ? 1 : -1);
			line2_error2 -= 2 * line2_dx;
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
			line3_x += (t2.x > t1.x ? 1 : -1);
			line3_error2 -= 2 * line3_dx;
		}
		//Update line2 x coordinate
		line2_error2 += line2_derror2; 
		while(line2_error2 > line2_dy)
		{
			line2_x += (t2.x > t0.x ? 1 : -1);
			line2_error2 -= 2 * line2_dx;
		}

	}
}

