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

void WxWindDrawWind(uint8_t speedKph, uint8_t compassDir)
{
	uint16_t display_count = M5.getDisplayCount();
	Serial.printf("there are %d displays available()\n", display_count);
	
    M5.Lcd.fillScreen(BLACK); // Clear screen


    // Get dimensions
    uint16_t screenWidth = M5.Lcd.width()-1;
    uint16_t screenHeight = M5.Lcd.height()-1;
	uint16_t radius = min(screenWidth, screenHeight) /2;
	
    M5.Lcd.fillCircle(screenWidth/2, screenHeight/2, radius, CYAN);
    M5.Lcd.fillCircle(screenWidth/2, screenHeight/2, radius - 16, BLACK);

    //M5.Display.setCursor(screenWidth/2, screenHeight/2);
#if 0  


	int16_t x=1, y=2;
	int16_t  x1, y1;
	uint16_t w, h;
	
	//M5.Lcd.getTextBounds("FOO", x, y, &x1, &y1, &w, &h);
    
    // https://github.com/m5stack/M5Stack/blob/master/src/utility/In_eSPI.h#L554
    M5.Lcd.setTextDatum(CC_DATUM);  // center horiz + center vert aka CC
	M5.Lcd.printf("hi");

//	M5.Lcd.drawString("M5Stack has been connected", 0, 0, 3);

	//tft.drawString("234", screenWidth/2, screenHeight/2, 4);

    // M5.Display.printf("%d\n", speedKph);
#else
	M5.Lcd.setTextColor(TFT_WHITE, TFT_BLACK);
	
	 M5.Lcd.setTextDatum(TC_DATUM);  // Centre text on x,y position
	
     //https://m5stack.lang-ship.com/howto/m5gfx/font/   //TIP
     M5.Lcd.setTextSize(3);
     M5.Lcd.setFreeFont(&fonts::FreeMonoBold24pt7b);
     
 	 char msg[200];
	 sprintf(msg, "%2d", speedKph);

 	 uint16_t cHeight = M5.Lcd.fontHeight(&fonts::FreeMonoBold24pt7b);
	 
	 M5.Lcd.drawString(
		 msg, 
		 screenWidth/2, screenHeight/2- cHeight/2,
		 &fonts::FreeMonoBold24pt7b); //GFXFF);  // Draw the text string in the selected GFX free font


    M5.Lcd.display();
#endif

}

