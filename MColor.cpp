// MColor.cpp - Calculate Colors for temperature-values to be displayed
// V 1.0 - OM 200301
// using HSL-Color-Wheel for calculating color palette
// ToDo: use optimized color palette

#include "MColor.h"

int _colpalRangeSpreadAct = 1;
int _colpalRangeSpread[5][2] = {{1000, 400}, { MC_TMINRANGE_SPREAD,  MC_TMAXRANGE_SPREAD}, {300, 200}, {100, 100}, {0,0}}; 

void MC_setColorPalRange(int cp)    {   _colpalRangeSpreadAct = cp;}
int  MC_getColorPalRange()          {   return _colpalRangeSpreadAct;}
void MC_toggleColorPalRange()       {   ((_colpalRangeSpreadAct < 4) ? _colpalRangeSpreadAct++ : (_colpalRangeSpreadAct = 0)); }
int  MC_getRangeMin(int tmin)       {   return MTools_redprec(_max(MC_TRANGEBOT, tmin - _colpalRangeSpread[_colpalRangeSpreadAct][0]), 2); }
int  MC_getRangeMax(int tmax)       {   return MTools_redprec(_max(MC_TRANGETOP, tmax + _colpalRangeSpread[_colpalRangeSpreadAct][1]), 2); }

int MC_getColorHSL2RGB(int value, int vmin, int vmax, int vavg, int sat, int lum)
{
    int ch = MC_getColorHSL(value, vmin, vmax, vavg);    
    return MC_hsv2rgb(ch, (float) sat/100, (float) lum/100);  // needs float...
}
int MC_getColorHSL(int value, int vmin, int vmax, int vavg)
{		
    // do not use 255 - 315 deg = 60 deg   --> it's not clearly 'felt' as cold or hot
    // vmin/vmax set here for minimum spread of spectrum -> less color flicker if no spots seen // (un)comment...
    if((vavg - vmin) < MC_TMIN_SPREAD2AVG)     { vmin = vavg-MC_TMIN_SPREAD2AVG; }
    if((vmax - vavg) < MC_TMIN_SPREAD2AVG)     { vmax = vavg+MC_TMIN_SPREAD2AVG; }    
    // map range vmin -> 315, vmax -> 15 degree 'left turn'
    int colInd = map(value, vmin, vmax, MC_COLORWHEEL_LOW, MC_COLORWHEEL_HIGH);  // exclude 315...15 = 60 deg 
	colInd = colInd - 60;                           // counterclockwise turn - 60: icecold should be DEEPBLUE = 255, hot DARKRED/VIOLET 315
	if (colInd < 0) colInd += 360;	
	return colInd;
}

int MC_hsv2rgb(int h, float s, float v)
{
    double C, X, m;
    double r, g, b;
    double hh = (double) h / 60;
    
    C = (1 - abs(2 * v - 1)) * s;
    X = C * (1 - abs(fmod(hh, 2) - 1));
    m = v - C/2;        

    if (h >= 0 && h < 60) { r = C; g = X; b = 0; }
    if (h >= 60 && h < 120) { r = X; g = C; b = 0; }
    if (h >= 120 && h < 180) { r = 0; g = C; b = X; }
    if (h >= 180 && h < 240) { r = 0; g = X; b = C; }
    if (h >= 240 && h < 300) { r = X; g = 0; b = C; }
    if (h >= 300 && h < 360) { r = C; g = 0; b = X; }

    uint8_t R = round((r + m) * 255);
    uint8_t G = round((g + m) * 255);
    uint8_t B = round((b + m) * 255);    
    return ((R & 0xF8) << 8) | ((G & 0xFC) << 3) | (B >> 3);    
}