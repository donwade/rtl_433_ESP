#include <Arduino.h>
#include <ArduinoJson.h>
#include "WxUI.h"
#include "_m5Core2-only.h"

#include "pretty.h"
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

void json_433_Callback(char* jsonIn)
{
   deserializeJson(jsonDecoded,jsonIn);
  //logJson(jsonDecoded);

  uint16_t id = jsonDecoded["id"];
  switch (id)
  {
  	case 3870:
		serializeJsonPretty(jsonDecoded, Serial); Serial.print("\n");
  		Serial.printf(FG_GREEN "valid device detected\n\n" FG_DONE);
  	break;

  	default:
  		String foo =jsonDecoded["model"];
  		Serial.printf(FG_RED "unknown id %d ... %s\n" FG_DONE, id, foo.c_str());
  	break;
  }
}

void setup_WxUI(void)
{
	_setup_M5();
}

void loop_WxUI(void)
{
	_loop_M5();
}

