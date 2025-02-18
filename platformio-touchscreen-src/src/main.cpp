#include <Arduino.h>

void setup()
{
	Serial.begin(115200);
}

void loop()
{
	Serial.println("IDLE loop");
	delay(1000);
}