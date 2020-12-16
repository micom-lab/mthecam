// MTheCam - Mini Thermo Cam
// AMG8853 Grideye-Sensor with ESP32 MCU @M5StickC
// Version 'M5: Light' - V1.2 - Elektor
// Olaf Mertens 181213 - 1.2: 200501
// !!!!! do not forget: include ArduinoJson and M5STickC via lib-manager  !!!!!
// _ssid & _password needs to be set for your WIFI
// AMG sensor needs up to 15min for full precision 'warm up'

#define MTCVERSION      "MTC-M5/1.1"

#include <WiFi.h>
#include <WiFiClient.h>
#include "MTools1.h"
#include "MUI_Term.h"           // serial-terminal
#include "MDUI.h"               // TFT-Display
#include "MWUI.h"               // Web-UI 

#define MTCDISPINTERVALL        200     // ms 

// Change ssid and password with your personal WIFI credentials
const char* _ssid			    = "YourSSIDHere";
const char* _password		    = "YourPasswordHere";

const String    _deviceID		= "MTCM5";
unsigned long   _ts             = millis();         // check proc-time 

// SETUP and INIT 
void setup()
{      
    MTC_Tools_Stack_init();              // for debugging: Stack-calcs start here
    const char* version  = MTCVERSION;
    const char* hello1  = "MiniThermoCam"; 
    const char* hello2  = "- M5StickC -"; 
    const char* network = "not found!";

    MUIT_hello(hello1, hello2);     
    MTC_init(_deviceID);                // init MTC-Driver: Read and write to hardware registers	
    MD_init(PORTRAIT);                  // init TFT_Display
    MD_hello(hello1, hello2);           // hello world on TFT...
    MW_init();                          // init MWUI - Networking and Web-UI
    
    // join WLAN 
    WiFi.persistent(false);             // don't save WIFI-creds
    WiFi.mode(WIFI_STA);                // switch to ESP station mode          
    WiFi.begin(_ssid, _password);       // look for known WLAN & try to connect

    UI_TFTOut("", true);
    UI_TFTOut("Try WIFI:\n" + String(_ssid));    
    int cnt = 0;
    while ((WiFi.status() != WL_CONNECTED) && cnt++ < 30)   // Wait for connection - will need some time
    {
        delay(200);                      // 200ms delay while waiting
        UI_TFTOut(".", false);           // Display 'ping'
        MUIT_out(".", false);            // console-out for beeing alive
    }

    // if WIFI ok, start Webserver. TFT-output works fine even if WIFI is down!
    if(WiFi.status() == WL_CONNECTED)       {   MW_wsStart();   }       // ws crashes if WIFI not connected

    // some infos about WIFI-connection: UI_TFT... on TFT, MUIT... on serial terminal
    MUIT_out("x");                        // start with a new line....
    
    if(WiFi.status() == WL_CONNECTED )       { network = _ssid;}    
    UI_TFTOut("Connectd to: ", true);
    UI_TFTOut(network, true);    
    UI_TFTOut("", true);
    UI_TFTOut("IP address : ", true);
    UI_TFTOut(WiFi.localIP().toString(), true);  
    UI_TFTOut("", true);    
    UI_TFTOut("HTTP server: ", true);
    
    MUIT_outDebug("Setup", "Connected to " + WiFi.SSID() + " - IP address: " + WiFi.localIP().toString() + " -> HTTP server started");        
    UI_TFTOut("... start OK", true);
    delay(4000);  
    UI_TFTClear();  
    _ts = 0; 
}

// main loop 
void loop()
{       
    unsigned long tsrun = millis();             // loop-run time 
    MW_handleWSClient();                        // give webserver a chance to handle requests 
    if((millis() - _ts) > MTCDISPINTERVALL )    // every x ms    
    {               
        _ts = millis();       
        MTC_update();                           // get next frame from sensor
        if(MTC5_ButtonAPressed()) MC_toggleColorPalRange();         // play with ranges of colorpalette        
        UI_TFTFrameShow();                      // show on local display
        // show line on serial with infos - and used cpu-time
        MUIT_outDebug("^^^^ LOOP " + String(MTCVERSION) + " #" + String(millis()/1000),  
                                String("IP: ") + WiFi.localIP().toString() +
                                String(" - Bat = ") + String(MTC5_getBatVolt(), 2) + " V" + 
                                String(" - Dev = ") + String((float) MTC_getDeviceTemp()/100, 2) + " °C" +
                                String(" - Heap = ") + String(MTC_freeHeap())+ " min="+String(MTC_freeHeapMin()) +
                                String(" - Stack = ") + String(MTC_Tools_Stack_getUsed()) + 
                                String(" - run=") + String(millis() - tsrun) + " ms");       
    }                                    
}
