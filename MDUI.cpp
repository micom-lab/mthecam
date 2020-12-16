// MDUI.h - Show text/graphs on TFT-Display
// V 1.0 - OM 200301
// use with M5StickC

#include "MDUI.h"

dispmode _dispmode;     // Portrait or Landscape
disptile _disptile;     // part of screen (halfsize)

void MD_init(dispmode dmode)             // TFT setup
{
    _dispmode = dmode; 
    _disptile = ((dmode == LANDSCAPE) ? LEFT: TOP);
    M5.begin(true, true, false);        // Serial OFF -> no msgs from M5-Lib
    UI_TFTClear();                      // FULL clear here!   
    UI_TFTHome(dmode, _disptile);
}
void MD_hello(const char* t1, const char* t2)
{
    UI_TFTOutRepeat('-');
    UI_TFTOut(t1, true);
    UI_TFTOut(t2, true);
    UI_TFTOutRepeat('-');
}

// prepare display, reset 
void UI_TFTHome(dispmode dmode, disptile dtile)
{
    int x,y;
    _dispmode = dmode;
    _disptile = dtile;
    int rot = ((dmode == LANDSCAPE) ? 3:0);
    M5.Lcd.setRotation(rot);    
    M5.Lcd.setTextSize(1);
    M5.Lcd.setTextColor(TFT_COLOR_TEXT);
    M5.Lcd.setTextWrap(false);
    switch (dtile)
    {        
        case BOTTOM : x=0; y=TFT_TILEHALF; break;
        case LEFT   : x=0; y=0; break;
        case RIGHT  : x=TFT_TILEHALF; y=0; break;
        default     : x=0; y=0; break;
    }
    M5.Lcd.setCursor(x, y); 
    UI_TFTClear(dtile);    
}
// show text on TFT 
void UI_TFTOut(String text, boolean newline)        {   UI_TFTOut(text.c_str(), newline);    }
void UI_TFTOut(const char* text, boolean newline)
{
    if(newline) M5.Lcd.println(text);
    else        M5.Lcd.print(text);
}
void UI_TFTOutPos(int x, int y, String text)        // x or y == -1: use same line
{
    int xold = M5.Lcd.getCursorX();
    int yold = M5.Lcd.getCursorY();
    if (x == -1) x = xold;
    if (y == -1) y = yold;
    M5.Lcd.fillRect(x, y, TFT_FULLB, 10, TFT_BACKGROUND_DARK);  // clear old  
    M5.Lcd.setCursor(x, y);        
    M5.Lcd.print(text);
    M5.Lcd.setCursor(xold, yold);
}
// show line ------
void UI_TFTOutLine(disptile tile, String text, boolean newline)
{    
    if(tile == RIGHT)  {   M5.Lcd.setCursor(TFT_TILEHALF, M5.Lcd.getCursorY());}
    UI_TFTOut(text, newline);

}
// show repeated char 
void UI_TFTOutRepeat(const char s, int repeat)
{
    char line[TFT_CHARSPERLINE1+1] = {0};
    memset(line, (int) s, repeat);
    line[sizeof(line)-1] = 0;
    UI_TFTOut(line);    
}

// clear 'image' area halfscreen / full
void UI_TFTClear(disptile tile)
{
    M5.Lcd.setCursor(0, 0);  
    switch (tile)
    {
        case FULL:
            M5.Lcd.fillRect(0, 0, TFT_TILEHALF, TFT_TILEHALF, TFT_BACKGROUND_DARK);
             M5.Lcd.fillRect(0, TFT_TILEHALF, TFT_TILEHALF, TFT_TILEHALF, TFT_BACKGROUND_DARK);
            break;
        case LEFT:
            M5.Lcd.fillRect(0, 0, TFT_TILEHALF, TFT_TILEHALF, TFT_BACKGROUND_DARK);
            break;
        case RIGHT:
            M5.Lcd.fillRect(TFT_TILEHALF, 0, TFT_TILEHALF, TFT_TILEHALF, TFT_BACKGROUND_DARK);
            M5.Lcd.setCursor(TFT_TILEHALF, 0);  
            break;
        case TOP:        
            M5.Lcd.fillRect(0, 0, TFT_TILEHALF, TFT_TILEHALF, TFT_BACKGROUND_DARK);
            break;
        case BOTTOM:
            M5.Lcd.fillRect(0, TFT_TILEHALF, TFT_TILEHALF, TFT_TILEHALF, TFT_BACKGROUND_DARK);
            M5.Lcd.setCursor(0, TFT_TILEHALF);
            break;
    }
    
}
// show tempvalue-frame on tile
void UI_TFTFrameShow()
{
    unsigned long ts = millis();               // calc process time 
    int*  frame      = MTC_getFrame();    
    int   x, y;    
    int   tmin       = MTC_getTempMin();
    int   tmax       = MTC_getTempMax();
    int   tavg       = MTC_getTempAvg();      // not used yet        
    int   fsize      = MTC_getFrameSize();  
    int   rsize      = MTC_getRowSize();        
    int   tminRange	 = MC_getRangeMin(tmin);  // _max(0, tmin - MC_TMINRANGE_SPREAD);		// needed for scaling color-gradient into min-max range
	int   tmaxRange	 = MC_getRangeMax(tmax);  // _max(2500, tmax + MC_TMAXRANGE_SPREAD);	// spread: under min and over max border -> 'color gradient margin'	
    int   pixsize    = 10;
        
    UI_TFTOutPos(0, 5, "MiniThermoCam");
    // get color & paint colored rects
    x = 0; y = 15;
    for(int i = 0; i< fsize; i++)
    {                                
        int pixcol = MC_getColorHSL2RGB(frame[i], tminRange, tmaxRange, tavg);  // get color of tempvalue
        M5.Lcd.fillRect(x, y, pixsize, pixsize, pixcol);                  // fill rect with color
        x += pixsize;                                                     // next x-pos
        if(((i+1) % rsize == 0) && (i>0))    {   x = 0; y+=pixsize; }     // next y every 8 'i's only.
    }
    // show colorbar of used spectrum
    for(int i = 0; i < TFT_FULLB; i++ )
    {
        int gmin, max;
        float weight = 1.0;
        int tg = map(i, 0, 80, tmin, tmax); // tminRange, tmaxRange);
        int colRGB = MC_getColorHSL2RGB(tg, tminRange, tmaxRange, tavg);       
        M5.Lcd.fillRect(i, 98, 1, 10, colRGB);        
    }
    
    UI_TFTOutPos(0, 110, String(tminRange));
    UI_TFTOutPos(35, 110, "P"+String(MC_getColorPalRange()));   // temprang in colorange squeeze mode / experimental....
    UI_TFTOutPos(55, 110, String(tmaxRange));    
    UI_TFTOutPos(0, 130, "tMin= "+String(tmin));
    UI_TFTOutPos(0, 140, "tMax= " +String(tmax));
    UI_TFTOutPos(0, 150, "tAvg= " +String(tavg));

    MUIT_outDebug("UI_TFTFrameShow", "OK - " + String(millis()-ts) + " ms");
}

// // Make interpolated 32*32 'image' of an 8*8 pixel frame
// void UI_TFTmakeImage(int* frame, int* frameint)
// {   
//     MC_calcImage(frame, TFT_FRAMESIZE, frameint, TFT_FRAMESIZEINTER) ;	frame orig -> frame interpolated	    
// }

void UI_TFT_DispTest()
{
    M5.Lcd.setRotation(3); 
    M5.Lcd.setTextSize(1);
    M5.Lcd.setCursor(0, 3);    
    for(int i=0; i<8;i++)
    { 
        M5.Lcd.setCursor(0,i*10);
        M5.Lcd.print("*" + String(i) + "abcdefghijklmnopqr");
    }
}
