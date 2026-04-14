#ifndef _WXDATA
#define _WXDATA

#include <Arduino.h>
#include <TimeLib.h>
#include <TFT_eSPI.h>   // TFT colour def

extern char *getHHMMSS(uint32_t utc);

class ITEM{
public:
     ITEM() { assert(0);};

     ITEM(const char *name, uint32_t colour): valueLo(99), valueHi(-99), valUpdated(false), valueColour(colour)
     {
        strcpy(valueName, name);
     };

     uint32_t valueColour;

     char valueName [30];
	 float valueCurrent;
	 float valueLo;
     uint32_t timeLo;

	 float valueHi;
     uint32_t timeHi;

	 bool  valUpdated;
};

extern unsigned long getUTC();

#endif
