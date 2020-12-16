// MTCDevice.cpp
// comm with AMG88xx sensor
// based on panasonic app notes 
// V1.1 - OM 200301

#include "MTCDevice.h"
#include "MUI_Term.h"

char*   		_stack_start;						// debug: startpoint for stack-monitoring
unsigned long 	_lastSensorRead			= millis(); // sensor needs 100ms time befor next capture
boolean	 		_isCalibrated 			= false;
boolean	 		_isTestCalib			= false;
String 			_devName 				= "NN";
byte 			_rxBuf[BUF_LENRX];
byte 			_txBuf[BUF_LENTX];
int  			_frame[MTCFRAMESIZE];				// holds temp-values of last frame
int  			_framecalib[MTCFRAMESIZE];			// delta-values after calibration for correction
int				_frameinter[MTCFRAMESIZE_INTER];	// interpolated values of last frame
//int				_frameDiff[MTCFRAMESIZE];
int 			_tavg, _tmin, _tmax, _tdev;

// Prepare Hardware and I2C
void MTC_init(String devID)
{
	_devName = devID;
	pinMode(MTCINT, INPUT_PULLUP);			// INT-Handling: in LT not yet implemented
	pinMode(PIN_SDA, INPUT_PULLUP);
	pinMode(PIN_SCL, INPUT_PULLUP);	
	MTC_initSensor();			
	//MTC_initBat();						// solo-ESP32 / prepare ADC for batV reading
	MTC5_init();
}
void MTC_initSensor()
{
	Wire.begin(PIN_SDA, PIN_SCL);	
	MTC_reset(true);
	MTC_setFrameRate(FAST);					// FAST=10/s  SLOW=1/s 
	MTC_setMovingAvg(true);					// sensor-internal/less noise and more accuracy	
	MTC_initTVals();
	MUIT_outDebug("MTC_initSensor", "Sensor-reset/init done - frame initialized");
}
void MTC_initTVals()
{
	std::fill_n(_frame, MTCFRAMESIZE, -999);  // better for int[]	
	_tavg		= 0;						// average temp of frame
	_tmin 		= -999;					  	// find min...
	_tmax 		= 999;					  	// ... and max in frame	
	_tdev		= 0;						// sensor temp
}

String MTC_getDeviceID()	{ 	return _devName; }
void MTC_update()			{	MTC5_update(); MTC_makeFrame(); }
int* MTC_getFrame()			{	return _frame;}
int* MTC_getFrameInter()	{	return _frameinter;}
//int* MTC_getFrameDiff()			{	return _frameDiff;}  // online debug...
int  MTC_getRowSize()		{ 	return MTCROWSIZE; }
int  MTC_getRowSizeInter()  {	return MTCROWSIZEINTER; }
int  MTC_getFrameSize()		{ 	return MTCFRAMESIZE; }
int  MTC_getFrameSizeInter(){ 	return MTCFRAMESIZE_INTER; }
int  MTC_getTempMax()		{	return _tmax;}
int  MTC_getTempMin()		{	return _tmin;}
int  MTC_getTempAvg()		{	return _tavg;}
int* MTC_getCalibFrame()	{	return _framecalib; }

// reduce 'pixel' error by scanning an area with same temp all pix
// and calc&save correction-vals in a parallel array
// !!! remember: Sensor needs 15s after boot for 'burn in'.
void MTC_calibSensor()					
{	
	int calibFrames 	= 20;			// take avg of x frames
	int32_t frameavg[MTCFRAMESIZE] = {0};		// for avg value of each pix
	int32_t calibValue 	=  0;			// holds avg temp for all pix of x frames
	_isCalibrated	= false;
	for(int i = 0; i < calibFrames; i++)
	{
		MTC_makeFrame(true);			// here needed as raw - w/o using calibration
		calibValue += MTC_getTempAvg();	
		for(int k = 0; k < MTCFRAMESIZE; k++)		{	frameavg[k] += _frame[k];	}
		delay(150)	;					// longer then sensor-chip-reading!
	}	
	calibValue /= calibFrames;			// average of n frames average
	MUIT_outDebug("MTC_calibSensor", "Cal-Delta-Values: ("+String(calibValue)+")");
	for(int i = 0; i < MTCFRAMESIZE; i++)
	{
		_framecalib[i] = (int) (frameavg[i]/calibFrames) - (int) calibValue;		// save correction val of each avg px in new []
		//_framecalib[i] = _frame[i] - (int) calibValue;		// save correction val of each px in new []
		MUIT_outDebug("", String(i)+":"+ String(_framecalib[i]) +"~"+String(frameavg[i]/calibFrames) + String(((i>0) && (i%7 == 0)) ?"\n":" "), false);
	}	
	MUIT_outDebug("MTC_calibSensor", "calib done, in use");
	_isCalibrated = true;
}

// Read 'sizeRX' registers of sensor with address 'devAddr', starting at 'reg' 
// result goes back to rxBuff[]
boolean MTC_readReg(int devAddr, byte reg, byte rxBuf[], int sizeRX)
{
	boolean success = false;
	Wire.beginTransmission(devAddr);
	Wire.write(reg);			// 0x80 -> read 128 bytes	
	if(Wire.endTransmission()) 		{	return false;	}  // check here. call RESET on error
	
	// read sensor answer to rxBuf
	// you may evaluate error-return more exactly....
	if (Wire.readTransmission(devAddr, rxBuf, sizeRX) == I2C_ERROR_OK) { success = true; }
	Wire.endTransmission();
	return success;
}
// Temperature of the sensor itself
int MTC_getDeviceTemp()
{
	int deviceTemp = -9999;
	int devAddr = MTCADDRESS;
	byte reg = 0x0E;
	if (MTC_readReg(devAddr, reg, _rxBuf, 2)) 	{	deviceTemp = MTC_calcTemp(_rxBuf[0], _rxBuf[1], false);	}		
	else 										{	MTC_initSensor(); }
	return deviceTemp;
}

// read one frame with 64 temp-values in 12bit.
// 12bit means: 2 bytes are needed and joined for one value. 
// result is int[] with calculated values in _frame[] and interpolated in _frameinter[]
int* MTC_makeFrame(boolean raw)
{
	// no INT avl. for 'sensor-ready'
	// sensor updates in 100ms -> new read some ms later - feel free to use intervalltimer/INT
	if(! raw && ((millis() - _lastSensorRead) < MTCREADINTERVALL)) { return _frame; }
	unsigned long ts = millis();			
	
	int reg 	= 0x80;				// start wih this reg for 128 bytes
	boolean ok 	= false;

	// read 128 bytes - low & high 
	ok = MTC_readReg(MTCADDRESS, reg, _rxBuf, 128);
	if (ok)
	{
		byte dL[MTCFRAMESIZE];			// low byte
		byte dH[MTCFRAMESIZE];			// high byte	
		long tsum	= 0;				// to calc averag temp
		int	 k 		= 0;
		int  frsize 	= MTCFRAMESIZE;
		MTC_initTVals();				// init max/min etc.
		for (int i = 0; i < frsize; i++)
		{
			// if sensor is build in upside down - so start from end for [0] = top left.
			int index 	= MTCMOUNTDOWN ? (frsize - i - 1): i;
			int d1 		= _rxBuf[k++];		// low byte here
			int d2 		= _rxBuf[k++];		// hight byte here						
			// 2 byte were calculated into one integer
			int tpix = MTC_calcTemp(d1, d2, true);		
			//tpix = 360/64 * i *10;		// for test - whole color-space
			if(! raw)			// not done in calib-mode
			{	
				if(_isCalibrated)	{ 	tpix -= _framecalib[index];  }
				tpix = MTools_redprec(tpix, MTCSENSPREC); 
			}			
			// min/max: reset (not 0!) at start			
			_tmin = (i==0 ? tpix : _min(_tmin, tpix));
			_tmax = (i==0 ? tpix : _max(_tmax, tpix));
			tsum += tpix;
			_frame[index] = tpix;		// save temps in in array[]		
		}
		_tavg = (int) (tsum/frsize);			

		//if(usecalib) for (int i = 0; i < MTCFRAMESIZE; i++) _frameDiff[i] = _frame[i] - _tavg;  

		_lastSensorRead = millis();				// should this be here @end of I2C? 
		MTC_makeFrameInter();					// fills _frameinter[] with interpolated values
	}	
	else		
	{ 
		MTC_initSensor();		
	}

	MUIT_outDebug("MTC_makeFrame", "Sensor read: " +String(ok ? "OK" : "ERROR") + " - " + String(millis() - ts) + " ms");
	return _frame;							// _frame: global var will stay alive...
}

//calc interpolated values
void MTC_makeFrameInter()
{
	const uint8_t	rows		= MTCROWSIZE;
	const uint8_t	rowsInter	= MTCROWSIZEINTER;
	MCI_calcImage(_frame, rows, _frameinter, rowsInter);	 // makes 32*32 out of 8*8 pixels for a nicer image
}

// make one int from low/hight bytes v1/v2 of sensor
// return int of temperature in 1/100 Deg Celsius
// 'frame' means different factors for frame-values (true) or devicetemp-values (false)
int MTC_calcTemp(uint8_t v1, uint8_t v2, boolean frame)
{
	int16_t valRaw = 0;
	int16_t valCalc = 0;
	int factor = (int)(0.25 * 10000);				// this for frame-temp
	if (!frame) factor = (int)(0.0625 * 10000);		// this for devicetemp
	if (v2 & 0b00001000) v2 |= 0b11111000;
	valRaw = (v2 << 8) | v1;
	valCalc = (int)(valRaw * factor / 100);
	return valCalc;
}
void MTC_sleep()
{
	Wire.beginTransmission(MTCADDRESS);
	Wire.write(0x00);
	Wire.write(0x10);
	Wire.endTransmission();
}
void MTC_wakeUp()
{
	Wire.beginTransmission(MTCADDRESS);
	Wire.write(0x00);
	Wire.write(0x00);
	Wire.endTransmission();
}
void MTC_reset(boolean initial)
{
	Wire.beginTransmission(MTCADDRESS);
	Wire.write(0x01);
	if (initial) Wire.write(0x3F);
	else 		Wire.write(0x30);
	Wire.endTransmission();
}

void MTC_setFrameRate(int fr)
{
	Wire.beginTransmission(MTCADDRESS);
	Wire.write(0x02);
	if (fr == FAST) Wire.write(0x00);
	else			Wire.write(0x01);	
	Wire.endTransmission();	
}

// set moving average calculation (on sensor!) for better results and less noise
void MTC_setMovingAvg(boolean set)
{
	int r = 5;
	uint8_t reg[] = { 0x1f,0x1F,0x1f,0x07,0x1f };
	uint8_t val[] = { 0x50,0x45,0x57,0x20,0x00 };
	if (!set) val[3] = 0x00;
	for (int i = 0; i < r; i++)
	{
		Wire.beginTransmission(MTCADDRESS);
		Wire.write(reg[i]);
		Wire.write(val[i]);
		Wire.endTransmission();
	}
}

// ESP32-Battery analog -  not working on M5StickC!
void MTC_initBat()
{
    pinMode(MTC_PIN_ADC,INPUT);
    analogSetAttenuation(ATTENUATION);   // 6db
    analogReadResolution(RESOLUTION);
    analogSetClockDiv(255); // 1338 mS
}
// ESP32-Battery read voltage via ADC - in mV -  not working on M5StickC!
int MTC_ADCBat()
{
	// on board voltage divider brings bat to < 3.3V (max for ADC)
    int ADCBat = (int) (1000 * 3.30f / ADC_VOLTDIV * float(analogRead(PIN_ADC) / pow(2, RESOLUTION))) ;  // in mV	   
    return ADCBat;
}

int MTC_freeHeap()			{	return ESP.getFreeHeap(); }
int MTC_freeHeapMin()		{	return ESP.getMinFreeHeap(); }

void MTC_Tools_Stack_init(char* st_start)     // // init record of stack
{
  if(st_start == 0)
    {
        char stack;
        _stack_start = &stack;
    }
   else
    {   _stack_start = st_start;    }
}

int MTC_Tools_Stack_getUsed()         {   char stack;  return (_stack_start - &stack);    }
int MTC_Tools_Stack_getFree()         {   return 0; /* ESP.getFreeContStack() */ ; }

// M5StickC - stuff
void MTC5_init()
{	
	M5.Axp.begin();
	M5.Axp.EnableCoulombcounter();		// enable bat-cap balance req's
}
void MTC5_update()						{	M5.update();	}		// read HW (buttons A/B)
boolean MTC5_ButtonAPressed(int ptime)	{	return M5.BtnA.wasReleased();	}
float MTC5_getBatVolt()					{	return  M5.Axp.GetBatVoltage(); }
float MTC5_getBatPower()				{	return  M5.Axp.GetBatPower(); }