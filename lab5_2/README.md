| Supported Targets | ESP32-C3 |
| ----------------- | -------- |

# ADC Oneshot Morse Code Reciever

This file connects a Photoelectric Sensor to the ESP32-C3 and reads from it:

## How to use this file

### Hardware Required

* A development board with ESP SoC
* A USB cable for power supply and programming

In this example, you need to connect a voltage source (e.g. a DC power supply) to the GPIO pins specified in `main.c` which in our case is pin 3.

### Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py -p PORT flash monitor
```

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

### How to use

The photoelectric sensor reads from an LED. The variable THRESHOLD_VOLTAGE may be different for you, so you'll need to set it yourself. In my case it was 110 mV.   

The letters are printed one by one
