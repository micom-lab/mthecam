// MWUI.h
// UI for MTC-Output: Webserver, HTML-Doc and JSON-data
// uses ArduinoJson for JSON-stuff
// V1.1 - OM 200301

#ifndef _MWUI_h
#define _MWUI_h

#include <Arduino.h>
#include <ArduinoJson.h>
#include <WebServer.h>
#include "MTCDevice.h"
#include "MUI_Term.h"
#include "MColor.h"

void 	MW_init();                  // init Web-UI: HTML-Doc / Webserver
void    MW_wsStart();               // Webserver-Start 
void    MW_handleWSClient();        // let ws check on browser req's
// webserver - callback
void    MW_index();                 // handle request: / and /index.html 
void    MW_frameJS();               // ... /frame 
void    MW_calibrate();             // ... /MW_calibrate    - make & use calib-table based on x frames of 'no spot'
void    MW_reset();                 // ... /reset           - soft-reset sensor-chip
void    MW_notFound();              // ... catchall         - nothing to show
// 
int		MW_getColorHSL(int value, int vmin, int vmax);      // makes HSL-Color of temperature-value
String 	MW_getFrameJS();            // transfer sensor-frame to JSON-data 
const char* MW_getDocIndexPM();     // get HTML-doc from PROGMEM

#endif

