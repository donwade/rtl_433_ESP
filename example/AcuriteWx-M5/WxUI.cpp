#include <Arduino.h>
#include <ArduinoJson.h>
#include <TFT_eSPI.h>   // TFT colour def

typedef int TFT_COLOUR;

#include "WxData.h"
#include "WxUI.h"
#include "WxWind.h"

#include "_m5Core2-only.h"
#include "_viewController.h"
#include <Timezone.h>

#include "pretty.h"


TickType_t xMsgSysTick;

extern void WxWindDrawItem2(ITEM &item);


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


ITEM wind ("WIND", TFT_GREEN);
ITEM rain ("RAIN", TFT_CYAN);
ITEM temp ("TEMP", TFT_YELLOW);
ITEM hmdt ("HMDT", TFT_BLUE);


void json_433_Callback(char* jsonIn)
{
   deserializeJson(jsonDecoded,jsonIn);
  //logJson(jsonDecoded);

  float windNow;
  windNow = jsonDecoded["wind_avg_km_h"];
  uint16_t id = jsonDecoded["id"];

  static uint32_t lastTime = 0;
  uint32_t nw = millis();
  uint32_t diff = nw - lastTime;
  // messages are sent 3 in a row. Look for time b/n the bursts.
  diff > 10000 ? lastTime = nw : 0;
  
  
  switch (id)
  {
  	case 3870:
		serializeJsonPretty(jsonDecoded, Serial); Serial.print("\n");
  		Serial.printf(FG_GREEN "valid device detected win=%f \n\n" FG_DONE, windNow);
  		Serial.printf(FG_YELLOW "hi don %f\n", wind.valueCurrent);

		if (diff > 10000)
		{
			xMsgSysTick = xTaskGetTickCount();
			Serial.printf(FG_RED "last report time = %d mS  tickCount=%d\n" FG_DONE, 
					diff, xMsgSysTick);
		}

		
		if (windNow != wind.valueCurrent)
		{
			if (wind.valueHi < windNow)
			{
				wind.valueHi = windNow;
				wind.timeHi = now();
			}	
			
			if (wind.valueLo > windNow)
			{
				wind.valueLo = windNow;
				wind.timeLo = now();
			}				

  		}
  		Serial.printf("%s and %s\n", getHHMMSS(wind.timeLo),  getHHMMSS(wind.timeHi));
  		Serial.printf("%d and %d\n", wind.timeLo, wind.timeHi);
		wind.valUpdated = true;
		wind.valueCurrent = windNow;
  		
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

	while(true)
	{
		if (wind.valUpdated)
		{
			wind.valUpdated = false;

			//WxWindDrawItem("WIND", TFT_GREEN, wind.valueCurrent, wind.valueLo, wind.valueHi);
			WxWindDrawItem2(wind);

			delay(2000);
			WxWindDrawItem2(temp);
			
			delay(2000);
			WxWindDrawItem2(rain);

			delay(2000);
 			Serial.printf("WIND %.1f < %.1f < %.1f\n", wind.valueLo, wind.valueCurrent, wind.valueHi);
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
