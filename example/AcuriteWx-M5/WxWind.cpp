#include <M5Unified.h>
#include <M5GFX.h>

#include <Timezone.h>
#include <ArduinoJson.h>
#include <ArduinoLog.h>
#include <rtl_433_ESP.h>
#include "pretty.h"

#include "WxCommon.h"
#include "WxWind.h"
#include "WxUI.h"
#include "_m5Core2-only.h"

#include "Free_Fonts.h"
#include "gfxfont.h"
#include <Timezone.h>

// US Eastern Time Zone (New York, Detroit)
TimeChangeRule usEDT = {"EDT", Second, Sun, Mar, 2, -240};	// UTC - 4 hours
TimeChangeRule usEST = {"EST", First, Sun, Nov, 2, -300};	// UTC - 5 hours

//https://github.com/m5stack/M5Stack/blob/master/examples/Advanced/Display/Free_Font_Demo/Free_Font_Demo.ino

#define VALUE_FONT &fonts::FreeSansBold24pt7b
#define UNITS_FONT &fonts::FreeSansBold9pt7b
#define STATS_FONT &fonts::FreeMono12pt7b


char *getHHMMSS(uint32_t utc)
{
	static char msg[70];
	Timezone usEastern(usEDT, usEST);  // setup dst and timezone recipes

    time_t local = usEastern.toLocal(utc);
    //Serial.printf("************* %02d:%02d:%02d\n", hour(local), minute(local), second(local));
    
	struct tm *remote ;
	remote = localtime( &local );

	//https://www.geeksforgeeks.org/cpp/strftime-function-in-c/

	strftime(msg, sizeof(msg), "%I:%M%p", remote);
	return msg;
}

char *getDDMMYY(uint32_t utc)
{
	static char msg[70];
	Timezone usEastern(usEDT, usEST);  // setup dst and timezone recipes

    time_t local = usEastern.toLocal(utc);
    //Serial.printf("************* %02d:%02d:%02d\n", hour(local), minute(local), second(local));
    
	struct tm *remote ;
	remote = localtime( &local );

	//https://www.geeksforgeeks.org/cpp/strftime-function-in-c/

	strftime(msg, sizeof(msg), "%d/%b/%y", remote);
	return msg;
}

#define BTWEAK  6 // boarder tweak, keep text off edges.
#define VSPACE  6

void WxDrawWindDisplay(WIND_ITEM &item)
{
	uint16_t cHeight1 = 0;

	uint32_t foregnd;
	uint32_t backgnd;
	char msg[300];

	backgnd = BLACK;
	M5.Lcd.clear();

	// don't make 0 seem as a record. windspeed == 0 
	if ( item.valueCurrent == 0 || (item.valueCurrent < item.valueHi && item.valueCurrent > item.valueLo))
	{
		foregnd = item.valueColour;
	}
	else
	{
		// Setting a record !!!!
		foregnd = RED;
	}

	Serial.printf("%s \n" , getHHMMSS(getUTC()));

	Serial.printf("\tbackground = 0x%04X\n", backgnd);
	Serial.printf("\tforegound = 0x%04X\n", foregnd);
	
    // Get dimensions
    uint16_t screenWidth = M5.Lcd.width()-1;
    uint16_t screenHeight = M5.Lcd.height()-1;
	uint16_t radius = min(screenWidth, screenHeight) /2;
	
	M5.Lcd.setTextSize(1);

	//------------ vals -----------------
	// https://doc-tft-espi.readthedocs.io/tft_espi/datums/
	
	M5.Lcd.setTextDatum(TL_DATUM);  // top left
	sprintf(msg, "MIN=%.1f", item.valueLo);
	M5.Lcd.drawString(msg, BTWEAK, BTWEAK, STATS_FONT);	//set left edge of text

	M5.Lcd.setTextDatum(TC_DATUM);  // center on X
	M5.Lcd.drawString("NOW", screenWidth/2, BTWEAK, STATS_FONT); 


	M5.Lcd.setTextDatum(TR_DATUM);  // top right
 	sprintf(msg, "MAX=%.1f", item.valueHi);
	M5.Lcd.drawString(msg, screenWidth-BTWEAK , BTWEAK, STATS_FONT); //set right edge of text

	cHeight1 += M5.Lcd.fontHeight(STATS_FONT) + VSPACE;
	
	//-------------- time ---------------

	M5.Lcd.setTextDatum(TL_DATUM);  // top left
	sprintf(msg, "%s", getHHMMSS(item.timeLo));
	M5.Lcd.drawString(msg, BTWEAK, BTWEAK + cHeight1, STATS_FONT);	//set left edge of text
	
	M5.Lcd.setTextDatum(TR_DATUM);  // top right
	sprintf(msg, "%s", getHHMMSS(item.timeHi));
	M5.Lcd.drawString(msg, screenWidth-BTWEAK , BTWEAK + cHeight1, STATS_FONT); //set right edge of text

	cHeight1 += M5.Lcd.fontHeight(STATS_FONT) + VSPACE;
	
	//-------------- date ---------------

	M5.Lcd.setTextDatum(TL_DATUM);  // top left
	sprintf(msg, "%s", getDDMMYY(item.timeLo));
	M5.Lcd.drawString(msg, BTWEAK, BTWEAK + cHeight1, STATS_FONT);	//set left edge of text
	
	M5.Lcd.setTextDatum(TR_DATUM);  // top right
	sprintf(msg, "%s", getDDMMYY(item.timeHi));
	M5.Lcd.drawString(msg, screenWidth-BTWEAK , BTWEAK + cHeight1, STATS_FONT); //set right edge of text

	cHeight1 += M5.Lcd.fontHeight(STATS_FONT) + VSPACE; // ya odd

	//-----------------------------

	M5.Lcd.setTextDatum(TC_DATUM);  // Centre text on x,y position

	//https://m5stack.lang-ship.com/howto/m5gfx/font/   //TIP
	M5.Lcd.setTextSize(3);
	//M5.Lcd.setFont(VALUE_FONT);

	if (item.valueCurrent < 10.0)
		sprintf(msg, "%.1f", item.valueCurrent);
	else
		sprintf(msg, "%2d", (int) item.valueCurrent);
		

	// show value	 
	M5.Lcd.drawString(
		 msg, 
		 screenWidth/2, cHeight1,
		 VALUE_FONT);

	//-----------------------------
	cHeight1 += M5.Lcd.fontHeight(VALUE_FONT)/2 + VSPACE;

	// show units
	//M5.Lcd.setFont(UNITS_FONT);
	M5.Lcd.setTextSize(2);
 
 	cHeight1 += M5.Lcd.fontHeight(UNITS_FONT) + VSPACE *2;

	M5.Lcd.drawString(
		item.valueName, 
		screenWidth/2, cHeight1,
		 UNITS_FONT);

	M5.Lcd.display();
	//-----------------------------
}


void WxDrawTempDisplay(TEMP_ITEM &item)
{
	uint16_t cHeight1 = 0;

	uint32_t foregnd;
	uint32_t backgnd;
	char msg[300];

	backgnd = BLACK;

	// don't make 0 seem as a record. windspeed == 0 
	if ( item.valueCurrent == 0 || (item.valueCurrent < item.valueHi && item.valueCurrent > item.valueLo))
	{
		foregnd = item.valueColour;
		M5.Lcd.clear();
	}
	else
	{
		// Setting a record !!!!
		foregnd = RED;
 		M5.Lcd.fillScreen(backgnd); // Clear screen
	}

	Serial.printf("%s \n" , getHHMMSS(getUTC()));

	Serial.printf("\tbackground = 0x%04X\n", backgnd);
	Serial.printf("\tforegound = 0x%04X\n", foregnd);
	
    // Get dimensions
    uint16_t screenWidth = M5.Lcd.width()-1;
    uint16_t screenHeight = M5.Lcd.height()-1;
	uint16_t radius = min(screenWidth, screenHeight) /2;

	M5.Lcd.setTextSize(1);

	//M5.Lcd.setFont(STATS_FONT);

	// https://doc-tft-espi.readthedocs.io/tft_espi/datums/

	//------------ vals -----------------
	M5.Lcd.setTextDatum(TL_DATUM);  // top left
	sprintf(msg, "MIN=%.1f", item.valueLo);
	M5.Lcd.drawString(msg, BTWEAK, BTWEAK, STATS_FONT);	//set left edge of text

	M5.Lcd.setTextDatum(TC_DATUM);  // center on X
	M5.Lcd.drawString("NOW", screenWidth/2, BTWEAK, STATS_FONT); 


	M5.Lcd.setTextDatum(TR_DATUM);  // top right
 	sprintf(msg, "MAX=%.1f", item.valueHi);
	M5.Lcd.drawString(msg, screenWidth-BTWEAK , BTWEAK, STATS_FONT); //set right edge of text

	cHeight1 += M5.Lcd.fontHeight(STATS_FONT) + VSPACE;
	
	//-------------- time ---------------

	M5.Lcd.setTextDatum(TL_DATUM);  // top left
	sprintf(msg, "%s", getHHMMSS(item.timeLo));
	M5.Lcd.drawString(msg, BTWEAK, BTWEAK + cHeight1, STATS_FONT);	//set left edge of text
	
	M5.Lcd.setTextDatum(TR_DATUM);  // top right
	sprintf(msg, "%s", getHHMMSS(item.timeHi));
	M5.Lcd.drawString(msg, screenWidth-BTWEAK , BTWEAK + cHeight1, STATS_FONT); //set right edge of text

	cHeight1 += M5.Lcd.fontHeight(STATS_FONT) + VSPACE;
	
	//-------------- date ---------------

	M5.Lcd.setTextDatum(TL_DATUM);  // top left
	sprintf(msg, "%s", getDDMMYY(item.timeLo));
	M5.Lcd.drawString(msg, BTWEAK, BTWEAK + cHeight1, STATS_FONT);	//set left edge of text
	
	M5.Lcd.setTextDatum(TR_DATUM);  // top right
	sprintf(msg, "%s", getDDMMYY(item.timeHi));
	M5.Lcd.drawString(msg, screenWidth-BTWEAK , BTWEAK + cHeight1, STATS_FONT); //set right edge of text

	cHeight1 += M5.Lcd.fontHeight(STATS_FONT) + VSPACE;

	//-----------------------------

	M5.Lcd.setTextDatum(TC_DATUM);  // Centre text on x,y position

	//https://m5stack.lang-ship.com/howto/m5gfx/font/   //TIP
	M5.Lcd.setTextSize(3);
	//M5.Lcd.setFont(VALUE_FONT);

	if (item.valueCurrent < 10.0)
		sprintf(msg, "%.1f", item.valueCurrent);
	else
		sprintf(msg, "%2d", (int) item.valueCurrent);
		

	// show value	 
	M5.Lcd.drawString(
		 msg, 
		 screenWidth/2, cHeight1,
		 VALUE_FONT);

 	//-----------------------------
	cHeight1 += M5.Lcd.fontHeight(VALUE_FONT)/2 + VSPACE;

	// show units
	//M5.Lcd.setFont(UNITS_FONT);
	M5.Lcd.setTextSize(2);
 
 	cHeight1 += M5.Lcd.fontHeight(UNITS_FONT) + VSPACE *2;

	M5.Lcd.drawString(
		item.valueName, 
		screenWidth/2, cHeight1,
		 UNITS_FONT);

	M5.Lcd.display();


 }

void WxDrawRainDisplay(RAIN_ITEM &item)
{
	uint16_t cHeight1;

	uint32_t foregnd;
	uint32_t backgnd;
	char msg[300];

	foregnd = item.bValueChanged ? RED : item.valueColour;
	backgnd = BLACK;
	M5.Lcd.clear();

	M5.Lcd.fillScreen(backgnd);
 
	Serial.printf("%s \n" , getHHMMSS(getUTC()));

	Serial.printf("\tbackground = 0x%04X\n", backgnd);
	Serial.printf("\tforegound = 0x%04X\n", foregnd);
	
    // Get dimensions
    uint16_t screenWidth = M5.Lcd.width()-1;
    uint16_t screenHeight = M5.Lcd.height()-1;
	uint16_t radius = min(screenWidth, screenHeight) /2;
	
    //M5.Lcd.fillCircle(screenWidth/2, screenHeight/2, radius, CYAN);
    //M5.Lcd.fillCircle(screenWidth/2, screenHeight/2, radius - 8, BLACK);


	M5.Lcd.setTextSize(1);

	//M5.Lcd.setFont(STATS_FONT);

	// https://doc-tft-espi.readthedocs.io/tft_espi/datums/

	M5.Lcd.setTextDatum(TL_DATUM);  // top left
	sprintf(msg, "LAST=%.1f", item.oldRainfall);
	M5.Lcd.drawString(msg, BTWEAK, BTWEAK, STATS_FONT);	//set left edge of text

	//M5.Lcd.setTextDatum(TC_DATUM);  // center on X
	//M5.Lcd.drawString("NOW", screenWidth/2, BTWEAK, STATS_FONT); 


	M5.Lcd.setTextDatum(TR_DATUM);  // top right
 	sprintf(msg, "TOTAL=%.1f", item.valueSeenOnBoot);
	M5.Lcd.drawString(msg, screenWidth-BTWEAK , BTWEAK, STATS_FONT); //set right edge of text

	//-------time -----------------
	cHeight1 = M5.Lcd.fontHeight(STATS_FONT) + VSPACE;

	M5.Lcd.setTextDatum(TL_DATUM);  // top left
	sprintf(msg, "%s", getDDMMYY(item.timeNow));
	M5.Lcd.drawString(msg, BTWEAK, BTWEAK + cHeight1, STATS_FONT);	//set left edge of text
	
	//M5.Lcd.setTextDatum(TR_DATUM);  // top right
	//sprintf(msg, "%s", getDDMMYY(item.timeHi));
	//M5.Lcd.drawString(msg, screenWidth-BTWEAK , BTWEAK + cHeight1, STATS_FONT); //set right edge of text

	
	//------- date ----------------
	cHeight1 += M5.Lcd.fontHeight(STATS_FONT) + VSPACE;

	M5.Lcd.setTextDatum(TL_DATUM);  // top left
	sprintf(msg, "%s", getHHMMSS(item.timeNow));
	M5.Lcd.drawString(msg, BTWEAK, BTWEAK + cHeight1, STATS_FONT);	//set left edge of text
	
	//M5.Lcd.setTextDatum(TR_DATUM);  // top right
	//sprintf(msg, "%s", getHHMMSS(item.timeHi));
	//M5.Lcd.drawString(msg, screenWidth-BTWEAK , BTWEAK + cHeight1, STATS_FONT); //set right edge of text
	
	//--------- value --------------------
	
	cHeight1 += M5.Lcd.fontHeight(STATS_FONT) + VSPACE;

	M5.Lcd.setTextDatum(TC_DATUM);  // Centre text on x,y position

	//https://m5stack.lang-ship.com/howto/m5gfx/font/   //TIP
	M5.Lcd.setTextSize(3);
	//M5.Lcd.setFont(VALUE_FONT);

	sprintf(msg, "%.1f", item.oldRainfall);
		
	//cHeight1 += M5.Lcd.fontHeight(VALUE_FONT) /2;

	M5.Lcd.setTextColor(foregnd , backgnd);

	// show value	 
	M5.Lcd.drawString(
		 msg, 
		 screenWidth/2, cHeight1,
		 VALUE_FONT);

	//-----------------------------
	cHeight1 += M5.Lcd.fontHeight(VALUE_FONT)/2 + VSPACE;

	// show units
	//M5.Lcd.setFont(UNITS_FONT);
	M5.Lcd.setTextSize(2);
 
 	cHeight1 += M5.Lcd.fontHeight(UNITS_FONT) + VSPACE *2;

	M5.Lcd.drawString(
		item.valueName, 
		screenWidth/2, cHeight1,
		 UNITS_FONT);

	M5.Lcd.display();

}

/* TIP list of available fonts
FreeMono12pt7b.h			FreeSansBoldOblique12pt7b.h
FreeMono18pt7b.h			FreeSansBoldOblique18pt7b.h
FreeMono24pt7b.h			FreeSansBoldOblique24pt7b.h
FreeMono9pt7b.h				FreeSansBoldOblique9pt7b.h
FreeMonoBold12pt7b.h		FreeSansOblique12pt7b.h
FreeMonoBold18pt7b.h		FreeSansOblique18pt7b.h
FreeMonoBold24pt7b.h		FreeSansOblique24pt7b.h
FreeMonoBold9pt7b.h			FreeSansOblique9pt7b.h
FreeMonoBoldOblique12pt7b.h	FreeSerif12pt7b.h
FreeMonoBoldOblique18pt7b.h	FreeSerif18pt7b.h
FreeMonoBoldOblique24pt7b.h	FreeSerif24pt7b.h
FreeMonoBoldOblique9pt7b.h	FreeSerif9pt7b.h
FreeMonoOblique12pt7b.h		FreeSerifBold12pt7b.h
FreeMonoOblique18pt7b.h		FreeSerifBold18pt7b.h
FreeMonoOblique24pt7b.h		FreeSerifBold24pt7b.h
FreeMonoOblique9pt7b.h		FreeSerifBold9pt7b.h
FreeSans12pt7b.h			FreeSerifBoldItalic12pt7b.h
FreeSans18pt7b.h			FreeSerifBoldItalic18pt7b.h
FreeSans24pt7b.h			FreeSerifBoldItalic24pt7b.h
FreeSans9pt7b.h				FreeSerifBoldItalic9pt7b.h
FreeSansBold12pt7b.h		FreeSerifItalic12pt7b.h
FreeSansBold18pt7b.h		FreeSerifItalic18pt7b.h
FreeSansBold24pt7b.h		FreeSerifItalic24pt7b.h
FreeSansBold9pt7b.h			FreeSerifItalic9pt7b.h
*/
