#include <M5Unified.h>

// Function that gets current epoch time
unsigned long getUTC()
{
	time_t now;
	struct tm timeinfo;
	
	if (!getLocalTime(&timeinfo)) {
		//Serial.println("Failed to obtain time");
		return(0);
	}
	
	time(&now);
	return now;
}


