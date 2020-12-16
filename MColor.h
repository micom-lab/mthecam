// MColor.h - Calculate Colors for temperature-values to be displayed
// V 1.0 - OM 200301
// using HSL-Color-Wheel for calculating color palette
// 'Range'-Values for stretching to color spectrum
// ToDo: use optimized color palette
#pragma once
#ifndef _MCOLOR_h
#define _MCOLOR_h

#include <Arduino.h>
#include "MCalcInter.h"
#include "MTools1.h"

#define MC_COLORWHEEL_LOW       315    // degrees of HSL-color-wheel - felt as cold
#define MC_COLORWHEEL_HIGH      15     // degrees of HSL-color-wheel - felt as hot
#define MC_TMIN_SPREAD2AVG      500    // tmin & tmax should be 500 units away from taverage for less flicker on 'no spot seen'
#define MC_TMINRANGE_SPREAD     400    // 1000    	
#define MC_TMAXRANGE_SPREAD     300
#define MC_TRANGETOP            2500
#define MC_TRANGEBOT            -2000

int  MC_getColorPalRange();
void MC_setColorPalRange(int cp) ;
void MC_toggleColorPalRange() ;  
int MC_getRangeMin(int tmin);
int MC_getRangeMax(int tmax);
int MC_getColorHSL2RGB(int value, int vmin, int vmax, int vavg, int sat=100, int lum=50);
int MC_getColorHSL(int value, int vmin, int vmax, int vavg);
int MC_hsv2rgb(int h, float s, float v);

#endif