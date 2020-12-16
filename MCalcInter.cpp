// MCalcInter.cpp
// bicubic interpolation of frame of colorpixel
// Detail on https://en.wikipedia.org/wiki/Bicubic_interpolation
// edit OM 200301

#include <Arduino.h>
#include "MCalcInter.h"

// calulate interpolated 'image' dest for original src frame / quad: #row=#col
// input is INT[] - calc needs FLOAT[] 
void MCI_calcImage(int *src, const uint8_t src_rows, int *dest, const uint8_t dest_rows)
{
	// make FLOAT[] 
	const int sizeorig = src_rows * src_rows;		// as quad.
	const int sizeintpol = dest_rows * dest_rows;
	float f[sizeorig], fi[sizeintpol];	
	for (int i = 0; i < sizeorig; i++)	{ f[i] = (float) src[i]; } 
	MCI_calcImage(f, src_rows, fi, dest_rows);     // use interpolation allgo / needs FLOAT! ToDo:int[]
	// result to INT[]
	for (int i = 0; i < sizeintpol; i++)	{ dest[i] = (int)fi[i]; } 	
}
void MCI_calcImage(float *src, uint8_t src_rows, float *dest, uint8_t dest_rows)  // assume #row=#col
{
	uint8_t src_cols = src_rows;
	uint8_t dest_cols = dest_rows;
	float mu_x = (src_cols - 1.0) / (dest_cols - 1.0);
	float mu_y = (src_rows - 1.0) / (dest_rows - 1.0);

	float adj_2d[16]; 

	for (uint8_t y_idx = 0; y_idx < dest_rows; y_idx++) {
		for (uint8_t x_idx = 0; x_idx < dest_cols; x_idx++) {
			float x = x_idx * mu_x;
			float y = y_idx * mu_y;
			
			get_adjacents_2d(src, adj_2d, src_rows, src_cols, x, y);
			
			float frac_x = x - (int)x; 
			float frac_y = y - (int)y; 
			float out = bicubicInterpolate(adj_2d, frac_x, frac_y);
			
			set_point(dest, dest_rows, dest_cols, x_idx, y_idx, out);
		}
	}
}

float get_point(float *p, uint8_t rows, uint8_t cols, int8_t x, int8_t y) {
	if (x < 0)        x = 0;
	if (y < 0)        y = 0;
	if (x >= cols)    x = cols - 1;
	if (y >= rows)    y = rows - 1;
	return p[y * cols + x];
}
void set_point(float *p, uint8_t rows, uint8_t cols, int8_t x, int8_t y, float f) {
	if ((x < 0) || (x >= cols)) return;
	if ((y < 0) || (y >= rows)) return;
	p[y * cols + x] = f;
}

float cubicInterpolate(float p[], float x) {
	float r = p[1] + (0.5 * x * (p[2] - p[0] + x * (2.0*p[0] - 5.0*p[1] + 4.0*p[2] - p[3] + x * (3.0*(p[1] - p[2]) + p[3] - p[0]))));	
	return r;
}

float bicubicInterpolate(float p[], float x, float y) {
	float arr[4] = { 0,0,0,0 };
	arr[0] = cubicInterpolate(p + 0, x);
	arr[1] = cubicInterpolate(p + 4, x);
	arr[2] = cubicInterpolate(p + 8, x);
	arr[3] = cubicInterpolate(p + 12, x);
	return cubicInterpolate(arr, y);
}

void get_adjacents_1d(float *src, float *dest, uint8_t rows, uint8_t cols, int8_t x, int8_t y) 
{
	dest[0] = get_point(src, rows, cols, x - 1, y);
	dest[1] = get_point(src, rows, cols, x, y);
	dest[2] = get_point(src, rows, cols, x + 1, y);
	dest[3] = get_point(src, rows, cols, x + 2, y);
}

void get_adjacents_2d(float *src, float *dest, uint8_t rows, uint8_t cols, int8_t x, int8_t y) {	
	float arr[4];
	for (int8_t delta_y = -1; delta_y < 3; delta_y++) 
	{ 
		float *row = dest + 4 * (delta_y + 1); 
		for (int8_t delta_x = -1; delta_x < 3; delta_x++) 
		{ 
			row[delta_x + 1] = get_point(src, rows, cols, x + delta_x, y + delta_y);
		}
	}
}
