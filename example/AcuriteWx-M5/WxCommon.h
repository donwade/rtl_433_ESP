#ifndef _WXDATA
#define _WXDATA

#include <Arduino.h>
#include <TimeLib.h>
#include <TFT_eSPI.h>   // TFT colour def

extern char *getHHMMSS(uint32_t utc);

class HUMD_ITEM{
public:
     HUMD_ITEM() { assert(0);};

     HUMD_ITEM(const char *name, uint32_t colour): valueLo(99), valueHi(-99), bValChanged(false), valueColour(colour)
     {
        strcpy(valueName, name);
        valueCurrent = 9999.;
     };

     uint32_t valueColour;

     char valueName [30];
	 float valueCurrent;
	 float valueLo;
     uint32_t timeLo;

	 float valueHi;
     uint32_t timeHi;

	 bool  bValChanged;
};

class WIND_ITEM{
public:
     WIND_ITEM() { assert(0);};

     WIND_ITEM(const char *name, uint32_t colour): valueLo(99), valueHi(-99), valUpdated(false), valueColour(colour)
     {
        strcpy(valueName, name);
        valueCurrent = 9999.;
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



class TEMP_ITEM{
public:
     TEMP_ITEM() { assert(0);};

     TEMP_ITEM(const char *name, uint32_t colour): valueLo(99), valueHi(-99), valUpdated(false), valueColour(colour)
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

class RAIN_ITEM{
public:
     RAIN_ITEM() { assert(0);};

     RAIN_ITEM(const char *name, uint32_t colour): valueNow(99), bValueChanged(false), valueColour(colour)
     {
        strcpy(valueName, name);
        oldRainfall = 0;
        valueSeenOnBoot = 0;

     };

     uint32_t valueColour;
      char valueName [30];

     float valueSeenOnBoot;
     float oldRainfall;

     float valueNow;
     uint32_t timeNow;

//     float valueHi;
//     uint32_t timeHi;

	 bool  bValueChanged;
};

extern unsigned long getUTC();

#endif
