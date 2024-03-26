# rv - Garden Irrigation

## Automatic garden irrigation controlled by STM32/ESP32 pair of micro-controllers.

 It uses a pair of companion micro-controller-units, which watch and reset each other to prevent bad things from happening.

 I've built the final hardware in 2020 and used it since then in our garden. Nothing bad happened. I could not trust a single ESP32 with that task, for fear of flooding the garden.

### The critical work is done by an STM32F1 (blue-pill board) micro-controller:
 * Read state of rain sensor
 * The at_fault() handler will shut off all magnet valves to prevent flooding.
 * Provides a timer which schedules the irrigation zones.
 * Limits for water flow (fore each zone, and for a well pump) and water volume in a given time (for a well pump) can be set.
 * The timer will allow more than one zone to run at the same time, if the sum of the water flow does not exceed the pump limit.
 * If the garden pump did not need to run for a given time (2 days), it will turned on for a few seconds for rust protection.
 * Limits for manual water draw (hose bibs, water taps) duration will be enforced to prevent flooding
 * A user button allows extending the time limit temporarily, which will then reset to the normal limit after some hours.

### The higher level functions are provided by an ESP32 micro-controller. These are
 * MQTT client to connect to home server (For my garden, I use FHEM with alexa module for voice control)
 * HTTP server and Web-App as user-interface like networking (MQTT-client, HTTP-server), saving settings
 * Doing OTA firmware updates for the STM32
 * Fetching weather data every hour and store the data of the last seven days.
 * I currently work on weather data based full automatic irrigation (which I did with some crude FHEM user scripts for a few years)


### Required Hardware
* 24V AC transformer
* 12V DC power supply
* 12V 16-relay module board to control the 24V~ valves and to provide 5V for other hardware
* MCP23017 I2C IO-expander board running at 5V to control the relays
* STM32F1 (blue pill) micro-controller board (making its 3.3V from 5V and having lots of 5V tolerant pins (including I2C).
* ESP32 board (WLAN/LAN)

### How to compile
*  Building the ESP32 firmware requires ESP-IDF (v5.2) installed
*  Building the STM32 firmware requires one of the following:
   *  STM32CubeIDE (1.15.0) on Linux. libopencm3 is used. No HAL.
   *  make, cmake, gcc-arm-none-eabi, binutils-arm-none-eabi, python -m pip install esp-idf-kconfig

### Reusable libraries written for this project
* MCP23017 driver C-library for STM32F1-libopencm3
* esp-idf component to flash the STM32 over the air with help from the ESP32

### Author
Bert Winkelmann

### License
GPL v3
