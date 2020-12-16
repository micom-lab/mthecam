// MUI_Term.h 
// User interface Serial, LED.... 
// debug msgs / simple profiler
// V0.5 - OM 200204

#pragma once

#ifndef _MUI_h
#define _MUI_h

#include <Arduino.h>
#define OUTDEBUG true
#define OUTSERIAL true

#define CRLF                "\r\n"
#define MSERIAL_BAUD_HIGH	115200	// Highspeed
#define PROMPTDEBUG			String("$--| ")
#define TRENNLINIE1			String("=")
#define TRENNLINIE2			String("-")
#define CHARFILLMOD         '.'
#define LEADIN				">>> "
#define LEADOUT				"<<< "
#define TRENNANZAHL			40
#define GPIOLEDRUNNING		12
#define GPIOLEDTICK			13

void MUIT_begin(int baud = MSERIAL_BAUD_HIGH);
void MUIT_hello(const char* version = "PRE", const char* hello1 = "Hello World - App1", const char* hello2 ="");
void MUIT_out(String msg, boolean newline = true );
void MUIT_out(const char* msg, boolean newline = true);
void MUIT_outDebug(String modul, String msg, boolean newline = true);
void MUIT_outDebug(const char* modul, const char* msg, boolean newline = true);
void MUIT_show(String msg, boolean newline = true);
unsigned long MUIT_prof(String func ="");

#endif