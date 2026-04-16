#include <Arduino.h>
#include <SD.h>
#include <ArduinoJson.h>
#include "JsonIO.h"


void writeJsonToSD() 
{
    // 1. Create JSON data

    JsonDocument doc;
    doc["sensor"] = "gps";
    doc["time"] = 1351824120;

    JsonArray data = doc["data"].to<JsonArray>();
    data.add(48.756080);
    data.add(2.302038);

    // 2. Open file for writing
	
    File myFile = SD.open("/data.json", FILE_WRITE);
    if (myFile)
    {
    
        // 3. Serialize JSON to file
        if (serializeJson(doc, myFile) == 0) {
            Serial.println(F("Failed to write to file"));
        }
        myFile.close();
        Serial.println(F("JSON saved to SD card"));
    } else
    {
        Serial.println(F("Error opening data.json"));
    }
}

