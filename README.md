# esp32-touchscreen-home-assistant-panel

Using a https://www.waveshare.com/wiki/ESP32-S3-Touch-LCD-7 v1.1 touchscreen to control home assistant devices in PlatformIO with Arduino

[Commit fbf03f54052caf640e248d55e45f4b4075559762](https://github.com/PockyBum522/esp32-touchscreen-home-assistant-panel/commit/15c504be1ab20fb5113068617bda0e418f509b81) is a good commit to start at if you just need to get the 7-inch LCD board running in PlatformIO


Issues:

	* Random screen vertical shifting when LVGL is animating
	
	* I2C communication errors (See note at top of main.cpp, power-cycling will fix this after a few attempts)