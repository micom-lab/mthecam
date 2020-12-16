// MWUI.cpp
// UI for MTC-Output: Webserver, HTML-Doc and JSON-data
// uses ArduinoJson for JSON-stuff
// V1.1 - OM 200301

#include "MWUI.h"
#include "MWUI1-1.h"		// single HTML-doc in PROGMEM

WebServer       _webServer(80);

void MW_init()						{ 	; }   // lock @ MW_wsStart

void MW_wsStart()					// prepare & start Webserver
{	
	// tell webserver, what to do if browser request is coming in
	_webServer.on("/", MW_index);                // if browser line = "/", call M_index()
    _webServer.on("/index.html", MW_index);      // ...same if "/index.html"
	_webServer.on("/frame", MW_frameJS);         // output data as JSON-Data 	
    _webServer.on("/calibrate", MW_calibrate);   // sensor calibration (offset redu) 	
    _webServer.on("/reset", MW_reset);           // soft reset sensor chip
	_webServer.onNotFound(MW_notFound);
	_webServer.begin();                         // webserver starts here
}

void MW_handleWSClient()				{	_webServer.handleClient();}	// ws looks for browser requests & delivers HTML-doc

// 'Small'-UI for Smartphones
// these were called from webserver if a browser request was received
void MW_index()
{
    unsigned long ts0 = millis(); 	
    _webServer.send_P(200, "text/html", MW_getDocIndexPM()); 
    MUIT_outDebug("MW_index", "webserver request /  -> OK - "+ String(millis()-ts0) + "ms");
}

// response data as JSON - this is the API to Apps, too.
void MW_frameJS() 
{  
    unsigned long ts0 = millis();      	
	//_webServer.sendHeader("Access-Control-Allow-Origin", "*");    // not needed if same server
    _webServer.send(200, "text/html", MW_getFrameJS());    
    MUIT_outDebug("MW_frameJS", "webserver request /frame -> OK - " + String(millis()-ts0) + "ms");
}

void MW_calibrate()
{  
    unsigned long ts0 = millis();      		    
    MTC_calibSensor();
	MW_index();
    MUIT_outDebug("MW_calibrate", "webserver request /calibrate -> OK - " + String(millis()-ts0) + "ms");
}
void MW_reset() 
{
    MW_index();      // there is live after reset... 
    delay(3000);
    ESP.restart();    
}

// handle doc error
void MW_notFound()
{        
    _webServer.send(404, "text/html", "not found!");    
    MUIT_outDebug("M_notFound", "webserver request " + _webServer.uri()+" - NOT FOUND");
}
		
const char* MW_getDocIndexPM()		{	return _uidoc;}  // HTML-Doc in PROGMEM

// /frame = output as JSON-Dataset with real temp-values 
// and interpolated, mapped colors for image
// min, max, average, device-temp and bat-power added

String MW_getFrameJS()  
{		
	String devID	= MTC_getDeviceID();
	int* thFrame	= MTC_getFrame();			// read sensor-chip data
	int* thImage	= MTC_getFrameInter();		// interpolated values
	int tmin		= MTC_getTempMin();			// min in current frame
	int tmax		= MTC_getTempMax();			// max in current frame
	int tavg		= MTC_getTempAvg();			// calc'd in MTC_getFrame()
	int tdev 		= MTC_getDeviceTemp();		// chip temp (not needed)	
	int tminRange	= MC_getRangeMin(tmin); 	// needed for scaling color-gradient into min-max range
	int tmaxRange	= MC_getRangeMax(tmax); 	// spread: under min and over max border -> 'color gradient margin'	
	int rsize		= MTC_getRowSize();
	int rsizeInter	= MTC_getRowSizeInter();
	int fsize		= MTC_getFrameSize();
	int fsizeInter	= MTC_getFrameSizeInter();
	int batStatus	= (int) (MTC5_getBatVolt() *100);
	
	DynamicJsonDocument _jsDoc(20000);	
	_jsDoc["device"] 	= devID;
	_jsDoc["ver"]		= "1.0";
	_jsDoc["rowsize"]	= rsize;
	_jsDoc["rowsizeimage"]	= rsizeInter;
	_jsDoc["batvolt"]	= (float) batStatus/100.00; // String(batStatus, 2); // "0.0";  // _batVoltage;
	_jsDoc["tavg"]  	= tavg;
	_jsDoc["tmin"]		= tmin;
	_jsDoc["tmax"]		= tmax; 
	_jsDoc["tminrange"]	= tminRange;
	_jsDoc["tmaxrange"]	= tmaxRange;

	JsonArray frame 	= _jsDoc.createNestedArray("frame");
	JsonArray framecol 	= _jsDoc.createNestedArray("framecol");
	for (int i = 0; i < fsize; i++)
	{
		frame.add(thFrame[i]);		
		framecol.add(MC_getColorHSL(thFrame[i], tminRange, tmaxRange, tavg));	
	}
	
	JsonArray framecolinter	= _jsDoc.createNestedArray("framecolinter");
	for (int i = 0; i < fsizeInter; i++)
	{
		framecolinter.add(MC_getColorHSL(thImage[i], tminRange, tmaxRange, tavg));			
	}

	String jsData;
	serializeJson(_jsDoc, jsData); 	
	//Serial.println(_jsDoc.memoryUsage());	
	return jsData;
}
