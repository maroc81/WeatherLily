# WeatherLily
Internet weather station for the LilyGO Lily Pi


## Overview
Weather Lily is a simple internet weather station for the LilyGO Lily Pi.

Current features include:

- Current time
- Current temperature
- Current conditions
- Multi-day forecast with lo/high temperatures, probability of precipitation, forecasted precipitation amount


## Running

### Prerequisites
 - Visual Studio Code with PlatformIO extension (recommended)
   OR
   PlatformIO Core CLI installed
 - Lily Pi connected over USB to your computer
   - On linux, you will need permissions to access the USB device, this can be done by adding yourself to the dialout group
   ```
   sudo usermod -a -G dialout USERNAME
   ```
   OR (temporarily) by changing the permissions of the usb device
   ```
   sudo chmod a+rw /dev/ttyACM0
   ```

### Build and Upload
 1. Clone this repo including submodules
    ```
    git clone --recurse-submodules  https://github.com/maroc81/WeatherLily.git
    ```
    NOTE: If you cloned without submodules, you can initialize after cloning with the command
    ```
    git submodule update --init
    ```
 1. Open the root cloned directory with vscode
 1. Edit appConfig.h and set your WIFI, open weather map API key, zip code, and time zone
 1. Click the PlatformIO sidebar and click "Full Clean", followed by "Build", and finally "Upload".

 ## PlatformIO, LVGL, and ESPDIF
This project is one of the few (maybe only?) that uses PlatformIO with the ESPIDF framework and LVGL for the Lily Pi.

## Lily Pi

The LilyGo Lily Pi is an ESP32 based board with a 3.5" capacitive touchscreen LCD.

Resources:
 - LilyGO product webpage: http://www.lilygo.cn/prod_view.aspx?TypeId=50032&Id=1353&FId=t3:50032:3
 - Banggood: https://www.banggood.com/LILYGO-LILY-Pi-ESP32-WiFi-bluetooth-3_5-Inch-Capacitive-Touch-Screen-with-5V-2A-Relay-USB-Expansion-Port-p-1741427.html
 - LilyGO example source: https://github.com/Xinyuan-LilyGO/TTGO_TWatch_Library/tree/master/examples/LilyPi
 - ESP-IDF: https://docs.espressif.com/projects/esp-idf/en/latest/esp32/
