// MTCDevice.h

#ifndef _MTCDEVICE_h
#define _MTCDEVICE_h

#include <Arduino.h>
#include <Wire.h>
#include "MCalcInter.h"
#include "MTools1.h"
#include <M5StickC.h>
//#include "AXP192.h"

// Sensor AMG883x
#define MTCSENSPREC         1           // int-'round' - digit e.g.: 0= 1234, 1= 1230, 2= 1200 .... reduce prec for noise-red.
#define MTCMOUNTDOWN        true        // true = chip mounted upside down
#define MTCADDRESS			(0x68)
#define MTCROWSIZE			8
#define MTCROWSIZEINTER		32
#define MTCINT				25
#define MTCREADINTERVALL    120         // sensors needs 100ms -> min intervall 
#define MTCFRAMESIZE		(MTCROWSIZE * MTCROWSIZE)
#define MTCFRAMESIZE_INTER  (MTCROWSIZEINTER * MTCROWSIZEINTER)
#define BUF_LENRX			(MTCFRAMESIZE*2)   // 2 Bytes each measure
#define BUF_LENTX			10
#define SLOW				1
#define FAST				0
// ESP32-section
#define PIN_SDA             0
#define PIN_SCL             26
#define MTC_PIN_ADC         34
#define ADC_VOLTDIV         0.80  // 1.26  // (0.935f * (float) ((44.2 + 137)/137))
#define PIN_ADC             34
#define ATTENUATION         ADC_11db
#define RESOLUTION          10
// M5StickC
#define PIN_BUT0            37


void    MTC_init(String devID);
void    MTC_initSensor();
void    MTC_initTVals();
void    MTC5_init();
void    MTC_update();
int*    MTC_getFrame();
int*    MTC_getFrameInter();
int*    MTC_getFrameDiff();
void    MTC_calibSensor();
void    MTC_readSensor(int _thFrame[]);
int     MTC_getRowSize();
int     MTC_getRowSizeInter();
int     MTC_getFrameSize();
int     MTC_getFrameSizeInter();
String  MTC_getDeviceID();

boolean MTC_readReg(int devAddr, byte reg, byte rxBuf[], byte txBuf[], int sizeRS, int sizeTX=1);
int     MTC_getDeviceTemp();
int*    MTC_makeFrame(boolean raw = false);
int*    MTC_getCalibFrame();
void    MTC_makeFrameInter();
int     MTC_getPixelTemp(byte pix, boolean reverse = true);
int     MTC_getTempMax();
int     MTC_getTempMin();
int     MTC_getTempAvg();
int     MTC_calcTemp(uint8_t v1, uint8_t v2, boolean frame = true);
void    MTC_setFrameRate(int fr = FAST);
void    MTC_setMovingAvg(boolean set = true);
void    MTC_reset(boolean initial = true);
void    MTC_sleep();
void    MTC_wakeUp();
void    MTC_initBat();
int     MTC_ADCBat();
int     MTC_freeHeap();    
int     MTC_freeHeapMin();
void    MTC_Tools_Stack_init(char* st_start=0);
int     MTC_Tools_Stack_getUsed();
int     MTC_Tools_Stack_getFree();


void    MTC5_update();
boolean MTC5_ButtonAPressed(int ptime = 50);  // min 500ms pressed
float   MTC5_getBatVolt();
float   MTC5_getBatPower();

#endif

