// MUI_Term.cpp
// User interface Serial, LED.... 
// debug msgs / simple profiler
// V0.5 - OM 200204

#include 	"MUI_Term.h"
#define 	DEBUG_MODUL_DISP_LEN	30

boolean _isOpenSerPort = false;

void MUIT_begin(int baud)
{
    if(! _isOpenSerPort) 
    { 
        Serial.begin(MSERIAL_BAUD_HIGH);
		Serial.flush();    
        _isOpenSerPort = true;    
    }
}

void MUIT_hello(const char* version, const char* hello1, const char* hello2)
{
    char line[35];
    memset(line, '-', sizeof(line));
    line[sizeof(line)-1] = 0;

    MUIT_out("\n", true);
	MUIT_out(line, true);
	MUIT_out(String(hello1) + " "+String(version), true);   
	MUIT_out(hello2, true);
	MUIT_out(line, true);
}

void MUIT_outDebug(String modul, String msg, boolean newline)   { MUIT_outDebug(modul.c_str(), msg.c_str(), newline);}
void MUIT_outDebug(const char* modul, const char* message, boolean newline)
{
    if(! OUTDEBUG)  return;

	if(newline)
	{       
        String ts = String(millis());
        // debug line on terminal in format: [modulname.......<timestamp>] this message to show
        char line[DEBUG_MODUL_DISP_LEN + 1] = {0};
        memset(line, (int) CHARFILLMOD, DEBUG_MODUL_DISP_LEN);  
        line[DEBUG_MODUL_DISP_LEN - strlen(modul)] = 0; 
        char buff[256] = {0}; 
        strcat(buff, "[");
        strcat(buff, (char*) modul);
        strcat(buff, line);
        // show millis for used proc time - @debugtime
            //strcat(buff, "<");
            //strcat(buff, ts.c_str());
            //strcat(buff, ">");
        strcat(buff, "] ");
        strcat(buff,  message);
        MUIT_out(buff, newline);        		     
    }
    else        {   MUIT_out(message, newline);    }    	
}

void MUIT_out(String msg, boolean newline)      {   MUIT_out(msg.c_str(), newline); }
void MUIT_out(const char* msg, boolean newline)
{
    MUIT_begin();
    if(OUTSERIAL) 
        {
            Serial.print(msg);
            if(newline) Serial.println();
        }
    //else    UI_Show(msg, newline);
}

//void UI_Show(String msg, boolean newline){}// other display goes here}

unsigned long MUIT_prof(String func)  // simple profiler: elapsed time between fct call & return
{
	static unsigned long ts;
	unsigned long elap;
	if(func !="")                      // if "" -> start timer, else -> show elapsed time
	{
		elap = millis()-ts;        
		MUIT_out(func + " T=" + String(millis()-ts));
	}
	else	ts = millis();
    return elap;
}