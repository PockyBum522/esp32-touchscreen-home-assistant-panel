# esp32-touchscreen-home-assistant-panel

Using a https://www.waveshare.com/wiki/ESP32-S3-Touch-LCD-7 v1.1 touchscreen to send MQTT messages to control devices in PlatformIO with Arduino

See:
https://github.com/esp-arduino-libs/arduino-esp32-sdk/issues/1
if you're interested in a play-by-play of what I modified to fix the vertical screen shifting issue.

Update: 
 * Per lovekull76 in the above thread, I've updated this repo to work with high_perf modified files. 

 * I've got the unmodified libraries from the waveshare wiki in esp32-touchscreen-home-assistant-panel/platformio-touchscreen-src/lib in this repo but adding the high_perf modified platform files would have been 4gb so in the interest of not making this repo gigantic here's what you'll need to do to get this to build:

 * This was all done on linux, but I'm sure can be modified for Windows.

1. Download and install Arduino IDE (I used 2.3.4 but it shouldn't matter, go for latest)

2. Preferences > Additional boards managers urls, add: https://espressif.github.io/arduino-esp32/package_esp32_index.json

3. Tools > Board > Boards manager, find esp32 by Espressif Systems and choose version 3.0.2 to install, and install

4. Now follow the "How to Use" section in this readme https://github.com/esp-arduino-libs/arduino-esp32-sdk which should describe where to find the root path of the arduino-esp32 SDK and what to overwrite some files in there with. Make sure you're overwriting with the 3.0.2-h if you downloaded 3.0.2 in the previous step. I have not tested versions other than 3.0.2 yet.

5. Once you've overwritten the files in the previous step, copy to esp32-touchscreen-home-assistant-panel/platformio-touchscreen-src/modified-high-perf-idf-release-5_1 in this repo.

6. Path verification - You should end up with this as a valid path, and then things should build:
esp32-touchscreen-home-assistant-panel/platformio-touchscreen-src/modified-high-perf-idf-release-5_1/tools/esp32-arduino-libs/idf-release_v5.1-bd2b9390ef/esp32s3

7. Modify your paths in platformio.ini to point to the correct path on your machine for:

platform_packages =
    platformio/framework-arduinoespressif32-libs @ symlink:///media/secondary/repos/esp32-touchscreen-home-assistant-panel/platformio-touchscreen-src/modified-high-perf-idf-release-5_1/tools/esp32-arduino-libs/idf-release_v5.1-bd2b9390ef

and 

lib_deps =
    symlink:///media/secondary/repos/esp32-touchscreen-home-assistant-panel/platformio-touchscreen-src/lib/ESP32_Display_Panel
    symlink:///media/secondary/repos/esp32-touchscreen-home-assistant-panel/platformio-touchscreen-src/lib/ESP32_IO_Expander
    symlink:///media/secondary/repos/esp32-touchscreen-home-assistant-panel/platformio-touchscreen-src/lib/lvgl
    
(Note that you'll need to retain symlink:// prefix in all cases. I used absolute paths but you may have luck with relative.)
