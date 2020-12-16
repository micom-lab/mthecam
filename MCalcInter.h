// MCalcInter.h
// bicubic interpolation of frame of colorpixel
// Detail on https://en.wikipedia.org/wiki/Bicubic_interpolation

#ifndef _MCALCINTER_h
#define _MCALCINTER_h

void    MCI_calcImage(int *src, const uint8_t src_rows, int *dest, const uint8_t dest_rows);  // int[] -> float[]
void    MCI_calcImage(float *src, uint8_t src_rows, float *dest, uint8_t dest_rows); // needs float[]

float   get_point(float *p, uint8_t rows, uint8_t cols, int8_t x, int8_t y);
void    set_point(float *p, uint8_t rows, uint8_t cols, int8_t x, int8_t y, float f);
void    get_adjacents_1d(float *src, float *dest, uint8_t rows, uint8_t cols, int8_t x, int8_t y);
void    get_adjacents_2d(float *src, float *dest, uint8_t rows, uint8_t cols, int8_t x, int8_t y);
float   cubicInterpolate(float p[], float x);
float   bicubicInterpolate(float p[], float x, float y);

#endif

