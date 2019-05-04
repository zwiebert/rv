# rv

## This is a ongoing microcontroller project to control garden irrigation valves

The valves are controlled by timers which can be set using local hardware or via MQTT

### Required Hardware
* 24V AC transformer
* 12V CD power supply
* 12V 16-relay module board to control the 24V~ valves and to provide 5V for other hardware
* MCP23017 I2C IO-expander board running at 5V to control the relays
* Two TM1638 UI boards running at 5V with 8 LED-digits, 8 buttons and 8 LEDs each
* STM32F1 (blue pill) microcontroller board (making its 3.3V from 5V and having lots of 5V tolerant pins (including I2C).
* STLINK v2 programmer/debug-Probe
* Some networking hardware like ESP8266 connected vial serial port for MQTT

### Required Software
* Atollic TrueStudio IDE used for programming and debugging. Makefiles will be added later. 

### Reusable libraries written for this project
* MCP23017 driver C-library for STM32F1-libopencm3
* TM1638 + board specific driver C-library for STM32F1-libopencm3

### Author
Bert Winkelmann

### License
GPL v3

