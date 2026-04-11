#include <M5Unified.h>
#include <ArduinoJson.h>
#include <ArduinoLog.h>
#include <rtl_433_ESP.h>

#include "WxUI.h"
#include "_m5Core2-only.h"
#include "WxWind.h"
#include <M5Unified.h>
#include <M5GFX.h>
#include "WxWind.h"

#include "Free_Fonts.h"
#include "gfxfont.h"


//https://github.com/m5stack/M5Stack/blob/master/examples/Advanced/Display/Free_Font_Demo/Free_Font_Demo.ino

#define VALUE_FONT &fonts::FreeSansBold24pt7b
#define UNITS_FONT &fonts::FreeSansBold9pt7b

void WxWindDrawWind(float speedKph, uint8_t compassDir)
{
	uint16_t display_count = M5.getDisplayCount();
	Serial.printf("there are %d displays available()\n", display_count);
	
    M5.Lcd.fillScreen(BLACK); // Clear screen


    // Get dimensions
    uint16_t screenWidth = M5.Lcd.width()-1;
    uint16_t screenHeight = M5.Lcd.height()-1;
	uint16_t radius = min(screenWidth, screenHeight) /2;
	
    //M5.Lcd.fillCircle(screenWidth/2, screenHeight/2, radius, CYAN);
    //M5.Lcd.fillCircle(screenWidth/2, screenHeight/2, radius - 8, BLACK);

	M5.Lcd.setTextColor(TFT_GREEN, TFT_BLACK);
	
	M5.Lcd.setTextDatum(TC_DATUM);  // Centre text on x,y position

	//https://m5stack.lang-ship.com/howto/m5gfx/font/   //TIP
	M5.Lcd.setTextSize(3);
	M5.Lcd.setFreeFont(VALUE_FONT);

	char msg[200];
	if (speedKph < 10.0)
		sprintf(msg, "%.1f", speedKph);
	else
		sprintf(msg, "%2d", (int) speedKph);
		
	 uint16_t cHeight1 = M5.Lcd.fontHeight(VALUE_FONT);

	// show value	 
	M5.Lcd.drawString(
		 msg, 
		 screenWidth/2, screenHeight/2- cHeight1/2,
		 VALUE_FONT);

	// show units

	M5.Lcd.setTextSize(2);

	uint16_t cHeight2 = M5.Lcd.fontHeight(UNITS_FONT);

	// !!! draw string is NOT subject to set cursor.
	//     drawing a string is not the same as printing a string
	///NO NO M5.Display.setCursor(screenWidth/2, screenHeight/2 + cHeight1 + cHeight2/2);

	M5.Lcd.drawString(
		"kph", 
		screenWidth/2, screenHeight/2 + cHeight1/2 - cHeight2/2,
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
