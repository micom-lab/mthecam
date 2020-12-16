// MDUI.h - Show on tiny TFT-Display
// V 1.0 - OM 200301
// use with M5StickC

#ifndef _MDUI_h
#define _MDUI_h

#include <Arduino.h>
#include <M5StickC.h>
#include "MTCDevice.h"
#include "MColor.h"
#include "MUI_Term.h"

#define TFT_FRAMESIZE           8
#define TFT_FRAMESIZEINTER      32
#define TFT_FULLA               160
#define TFT_FULLB               80
#define TFT_TILEHALF            80
#define TFT_CHARSPERLINE1       13
#define TFT_BACKGROUND_DARK     BLACK
#define TFT_BACKGROUND_LIGHT    WHITE
#define TFT_COLOR_TEXT          WHITE

enum disptile {FULL, LEFT, RIGHT, TOP, BOTTOM};     // part of the display: we use 2 parts for graph & text
enum dispmode {LANDSCAPE, PORTRAIT};                // display mode

void MD_init(dispmode dmode = LANDSCAPE);
void MD_hello(const char* t1, const char* t2);
void UI_TFTHome(dispmode dmode = LANDSCAPE, disptile tile = FULL);
void UI_TFTOut(const char* text, boolean newline = false);
void UI_TFTOut(String text, boolean newline = false);
void UI_TFTOutPos(int x, int y, String text);
// void UI_TFTShowStatus(String status);
void UI_TFTOutRepeat(const char s, int repeat = TFT_CHARSPERLINE1);
void UI_TFTOutLine(disptile tile, String text, boolean newline = false);
void UI_TFTClear(disptile dtile = FULL);
void UI_TFTFrameShow();                             // update image & infos 

int  UI_hsvtorgb(int h, float s, float v);          // TFT needs RGB-Colors, MTC uses HSL-Colors
void UI_TFTmakeImage(int* frame, int* frameint);    // interpolated image on TFT

#endif
