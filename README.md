# rv

## Project for automatic garden irrigation using an STM32/ESP32 pair of microcontrollers.

 It uses a pair of companion micro-controller-units, which watch and reset each other if something is wrong.
 I used this setup for a few years now, and no accidents happend. I would not trust a single ESP32 with that task, for fear of flooding the garden.
 
 The critical work is done by an STM32F1 (blue-pill board) microcontroller:
    * Turning garden pump on or off on demand. If no demand turn on every 2 days for rust protection.
    * Read state of rain sensor
    * Limit the pump on time for water taps to a few minutes. The time limit can be extended by a button which can be pressed by the user
 
 The higher level functions are provided by an ESP32 microcontroller. These are
    * MQTT client to connect to home server (For my garden, I use FHEM with alexa module for voice control)
    * HTTP server and Web-App as user-interface like networking (MQTT-client, HTTP-server), saving settings
    * Doing OTA firmware updates for the STM32
    * Fetching weather data every hour and store the data of the last seven days.
    * I currently work on weather data based full automaitc irrigation (which I did with some crude FHEM user scripts for a few years)
    
    
### Required Hardware
* 24V AC transformer
* 12V CD power supply
* 12V 16-relay module board to control the 24V~ valves and to provide 5V for other hardware
* MCP23017 I2C IO-expander board running at 5V to control the relays
* STM32F1 (blue pill) microcontroller board (making its 3.3V from 5V and having lots of 5V tolerant pins (including I2C).

### Required Software
* Atollic TrueStudio IDE used for programming and debugging. Makefiles will be added later. 

### Reusable libraries written for this project
* MCP23017 driver C-library for STM32F1-libopencm3
* esp-idf component to flash the STM32 over the air

### Author
Bert Winkelmann

### License
GPL v3

