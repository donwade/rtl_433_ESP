#include <Arduino.h>
#include <ArduinoJson.h>
#include <TFT_eSPI.h>   // TFT colour def

typedef int TFT_COLOUR;

#include "WxCommon.h"
#include "WxUI.h"
#include "WxWind.h"

#include "_m5Core2-only.h"
#include "_viewController.h"
#include <Timezone.h>

#include "pretty.h"
#include <TFT_eSPI.h>


TickType_t xMsgSysTick;

extern void WxDrawWindDisplay(WIND_ITEM &item);
extern void WxDrawRainDisplay(RAIN_ITEM &item);
extern void WxDrawTempDisplay(TEMP_ITEM &item);
extern void WxDrawHumdDisplay(HUMD_ITEM &item);



/* reference
{
  "model": "Acurite-5n1",
  "message_type": 49,
  "id": 3870,
  "channel": "A",
  "sequence_num": 0,
  "battery_ok": 1,
  "wind_avg_km_h": 6.7946,
  "wind_dir_deg": 202.5,
  "rain_mm": 772.66797,
  "mic": "CHECKSUM",
  "protocol": "Acurite 592TXR Temp/Humidity, 592TX Temp, 5n1 Weather Station, 6045 Lightning, 899 Rain, 3N1, Atlas",
  "rssi": -100,
  "duration": 135000
}
*/

/*

void setup() {
  const char* json = "{\"sensor\":\"gps\",\"time\":1351824120,\"data\":[48.756,2.302]}";
  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, json);

  if (error) {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.c_str());
    return;
  }

  const char* sensor = doc["sensor"]; // "gps"
  long time = doc["time"];           // 1351824120
  double latitude = doc["data"][0];  // 48.756
}

*/
//-------------------------------------------------------------

JsonDocument jsonDecoded;


WIND_ITEM wind ("WIND", RGB32toRGB565(0x00FF00));
RAIN_ITEM rain ("RAIN", RGB32toRGB565(0xFFFF00));

TEMP_ITEM temp ("TEMP C", RGB32toRGB565(0x00FFFF));
HUMD_ITEM humd ("HUMIDITY", RGB32toRGB565(0xFF00FF));


void json_433_Callback(char* jsonIn)
{
   deserializeJson(jsonDecoded,jsonIn);
  //logJson(jsonDecoded);


  static uint32_t lastTime = 0;
  uint32_t nw = millis();
  uint32_t diff = nw - lastTime;
  // messages are sent 3 in a row. Look for time b/n the bursts.
  //diff > 10000 ? lastTime = nw : 0;

  float tempNow;
  float windNow;
  float rainAbs;
  float humdNow;
  
  bool bHasTemp;
  bool bHasWind;
  bool bHasRain;
  bool bHasHumd;
  
  uint16_t id = jsonDecoded["id"];
  
  switch (id)
  {
  	case 3870:

		// retrigger 18 second wait window
		xMsgSysTick = xTaskGetTickCount();
		Serial.printf(FG_RED "last report time = %d mS  tickCount=%d\n" FG_DONE, diff, xMsgSysTick);

		// not all fields are always populated.
		// https://arduinojson.org/v6/api/jsonobject/containskey/

		bHasTemp= jsonDecoded.containsKey("temperature_C");
		bHasWind = jsonDecoded.containsKey("wind_avg_km_h");
		bHasRain = jsonDecoded.containsKey("rain_mm");
		bHasHumd = jsonDecoded.containsKey("humidity");
  	

		serializeJsonPretty(jsonDecoded, Serial); Serial.print("\n");
		

		// wind ---------------------------------------------------------------

		if (bHasWind)
		{
			windNow = jsonDecoded["wind_avg_km_h"];
			
	  		Serial.printf(FG_GREEN "Wind =%.1f \n" FG_DONE, windNow);
			if (windNow != wind.valueCurrent)
			{
				if (wind.valueHi < windNow)
				{
					wind.valueHi = windNow;
					wind.timeHi = getUTC();
				}	
				
				if (wind.valueLo > windNow)
				{
					wind.valueLo = windNow;
					wind.timeLo = getUTC();
				}				

	  		}

	  		Serial.printf("wind lo: %1.f kph at %s\n", wind.valueLo, getHHMMSS(wind.timeLo));
	  		Serial.printf("wind hi: %1.f kph at %s\n", wind.valueHi, getHHMMSS(wind.timeHi));
	  		
			wind.valUpdated = true;
			wind.valueCurrent = windNow;

		}

		// humd ---------------------------------------------------------------

		if (bHasHumd)
		{
			humdNow = jsonDecoded["humidity"];
			
	  		Serial.printf(FG_GREEN "humd =%.1f \n" FG_DONE, humdNow);
			if (humdNow != humd.valueCurrent)
			{
				if (humd.valueHi < humdNow)
				{
					humd.valueHi = humdNow;
					humd.timeHi = getUTC();
				}	
				
				if (humd.valueLo > humdNow)
				{
					humd.valueLo = humdNow;
					humd.timeLo = getUTC();
				}				

	  		}

	  		Serial.printf("humd lo: %1.f %% %s\n", humd.valueLo, getHHMMSS(humd.timeLo));
	  		Serial.printf("humd hi: %1.f %% %s\n", humd.valueHi, getHHMMSS(humd.timeHi));
	  		
			humd.bValChanged = true;
			humd.valueCurrent = humdNow;

		}


		// temperature ---------------------------------------------------------------

		if (bHasTemp)
		{
			tempNow = jsonDecoded["temperature_C"];
			
	  		Serial.printf(FG_GREEN "temp =%.1f \n" FG_DONE, tempNow);
			if (tempNow != temp.valueCurrent)
			{
				if (temp.valueHi < tempNow)
				{
					temp.valueHi = tempNow;
					temp.timeHi = getUTC();
				}	
				
				if (temp.valueLo > tempNow)
				{
					temp.valueLo = tempNow;
					temp.timeLo = getUTC();
				}				

	  		}

	  		Serial.printf("temp lo: %1.fC at %s\n", temp.valueLo, getHHMMSS(temp.timeLo));
	  		Serial.printf("temp hi: %1.fC at %s\n", temp.valueHi, getHHMMSS(temp.timeHi));

			temp.bValChanged = true;
			temp.valueCurrent = tempNow;

		}
		
		// rain ---------------------------------------------------------------
		
		if (bHasRain)
		{
			rainAbs = jsonDecoded["rain_mm"];
		
			// lock in absolute total rain that the tower has.
			if (!rain.valueSeenOnBoot && rainAbs)
			{
				rain.valueSeenOnBoot = rainAbs;
				Serial.printf(FG_BCYAN "locking in base rainfall %.1f\n" FG_DONE, rainAbs);
			}
			
			static int injectRain = 0;
			// testing if (injectRain < 80) injectRain+=10;
			
			rainAbs = rain.valueSeenOnBoot + injectRain;   // testing
			
			if (rain.valueSeenOnBoot && rainAbs)  // not all messages have rain
			{
				// message has a rain component 
				float rainfallNow = rainAbs - rain.valueSeenOnBoot;
				Serial.printf("abs rain = %f   diff rain = %f\n", rainAbs,  rainfallNow);
			
				if (rainfallNow != rain.oldRainfall)	// 
				{
					rain.valueNow = rainfallNow;
					rain.timeNow= getUTC();
					rain.oldRainfall = rainfallNow;
					Serial.printf(FG_GREEN "increasing rain by %.1f mm ... time %s \n", rainfallNow, getHHMMSS(rain.timeNow));
					rain.bValueChanged = true;
				}
				else
				{
					// it reported a rain event but amount did not change.
					// TODO averaging.
					Serial.printf(FG_YELLOW "rain stopped at %.1f mm ... time %s \n", rainfallNow, getHHMMSS(rain.timeNow));
				}
				
				rain.bValueChanged = true;
				
			}
		}
	
  		
  	break;

  	default:
  		String foo =jsonDecoded["model"];
  		Serial.printf(FG_RED "unknown id %d ... %s\n" FG_DONE, id, foo.c_str());
  	break;
  }
}


void task_WxUI(void *)
{
	wind.timeLo = now();    //test
	delay(3000);		    //test
	wind.timeHi = now();	//test

	#define DELAY 18000/3   // 18 sec 3 displays
	while(true)
	{
		if (wind.valUpdated)
		{
			wind.valUpdated = false;

			WxDrawWindDisplay(wind);
 			Serial.printf("WIND %.1f < %.1f < %.1f\n", wind.valueLo, wind.valueCurrent, wind.valueHi);
			delay(DELAY);
			
			WxDrawHumdDisplay(humd);
 			Serial.printf("HUMD %.1f < %.1f < %.1f\n", humd.valueLo, humd.valueCurrent, humd.valueHi);
			delay(DELAY);
			
			WxDrawTempDisplay(temp);
 			Serial.printf("TEMP %.1f < %.1f < %.1f\n", temp.valueLo, temp.valueCurrent, temp.valueHi);
			delay(DELAY);

			WxDrawRainDisplay(rain);
 			Serial.printf("RAIN %.1f\n", rain.valueNow );
			delay(DELAY);
		}
		delay(500);
	}
}



void task_WxRadio(void *)
{
	const uint32_t messageTimeMs = 18250;
	
	TickType_t copyTime = 0;
	
	while(true)
	{
		if (xMsgSysTick)
		{
			
		 	_colourBarX(0x00FF00, 40);
		 	copyTime = xMsgSysTick;
		 	
		 	uint32_t addTicks = pdMS_TO_TICKS(messageTimeMs - 1000);
		 	
		 	Serial.printf(FG_CYAN "IN : tc in %d + add %d = out %d ?\n" FG_DONE,xMsgSysTick, addTicks, xMsgSysTick + addTicks);
		 	
			vTaskDelayUntil( &copyTime,
							  addTicks);  //open 1 second earlier.

		 	Serial.printf(FG_CYAN "OUT : exit tc= %d\n" FG_DONE, xTaskGetTickCount());
			xMsgSysTick = 0;

			// window open
		 	_colourBarX(0x0000FF, 40);

			while(true)
			{
				// wait for new message to arrive.
 				if (xMsgSysTick)
 				{
 					break;
 				}
				vTaskDelay(pdMS_TO_TICKS(100));
			}
		}
		else
		{
			delay(100); // awaiting first message.
		}
	}
}

/*
BaseType_t xTaskCreatePinnedToCore(
    TaskFunction_t pvTaskCode,    // Function to run
    const char * const pcName,    // Task name
    const uint32_t usStackDepth,  // Stack size in words (not bytes)
    void * const pvParameters,    // Parameter to pass
    UBaseType_t uxPriority,       // Task priority
    TaskHandle_t * const pxCreatedTask, // Task handle
    const BaseType_t xCoreID      // Core ID (0 or 1)
);

*/

TaskHandle_t hWxUI = NULL;
TaskHandle_t hWxRadio = NULL;

void setup_WxUI(void)
{
	_setup_M5();
	_setup_lightbar();

	xTaskCreatePinnedToCore(task_WxUI,	// function name.
							"WindTsk", 	// name
							8000, 		// stack words.
							NULL,		// no params.
							8,			// priority 8
							&hWxUI,		// thread handle
							0			// run core X x=0,1,tskNO_AFFINITY
							);  
							
	xTaskCreatePinnedToCore(task_WxRadio,	// function name.
							"WindRadio", 	// name
							8000, 			// stack words.
							NULL,			// no params.
							8,				// priority 8
							&hWxRadio,		// thread handle
							0				// run core X x=0,1,tskNO_AFFINITY
							);  
}

/* TIP: how specifying HARD time for task to wake up at.
 vTaskDelayUntil is a FreeRTOS API function used to delay a task 
 until a specific ABSOLUTE time, 
 ensuring precise, consistent periodic execution. 
 Unlike vTaskDelay (relative delay), 
 vTaskDelayUntil maintains a constant frequency even if the task's processing time varies.
 
void vSomeTaskFunction( void * pvParameters )
{
    // Initialize with current tick count
    TickType_t xMsgSysTick = xTaskGetTickCount();
    const TickType_t xFrequency = pdMS_TO_TICKS( 100 ); // 100ms period

    for( ;; )
    {
        // Delay until 100ms after the last wake time
        vTaskDelayUntil( &xMsgSysTick, xFrequency );

        // Task code here
        printf("Task running\n");
    }
}

TIP using vTaskDelay to specify ticks instead of arduino delay(xms)
void vTaskFunction( void * pvParameters )
{
    // Block for 500ms.
    const TickType_t xDelay = 500 / portTICK_PERIOD_MS;

    for( ;; )
    {
        // Simply toggle the LED every 500ms, blocking between each toggle. 
        vToggleLED();
        vTaskDelay( xDelay );
    }
}

*/
