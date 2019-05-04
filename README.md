# rv

## This is a ongoing microcontroller project to control garden irrigation valves

The valves are controlled by timers which can be set using local hardware or via MQTT

### Hardware used
* 16 relay module board (12V with 5V output voltage) to control the 24V~ valves
* MCP23017 I2C IO-expander board running at 5V to control the relays
* Two TM1638 UI boards running at 5V with 8 LED-digits, 8 buttons and 8 LEDs each
* STM32F1 (blue pill) microcontroller board running at 3.3V with 5V tolerant pins.
* STLINK v2 programmer/debug-Probe
* Some networking hardware like ESP8266 connected vial serial port will be needed at some point

### Software
* Atollic TrueStudio IDE used for programming and debugging

### Newly written reusable libraries part of this project
* MCP23017 C-library for STM32F1-libopencm3
* TM1638 C-library for STM32F1-libopencm3

### Author
Bert Winkelmann

