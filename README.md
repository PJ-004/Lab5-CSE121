# Lab 5
This lab involves making a morse code detector using the ESP32C3 and Raspberry Pi4. You need the RPi.GPIO library for python which can be found here
https://pypi.org/project/RPi.GPIO/

To download it on Ubuntu/Debian use:  

`sudo apt-get install python3-rpi.gpio -y` \[Note: The -y is optional\]  

Also esp-idf needs to be set up  

## Setup
You will need a breadboard to set this up, or you will have to hold the LED and photoelectric sensor in place.

The LED should be connected to GPIO port 4 on the Raspberry Pi 4, the LED should have a resistor between it and the pin to not burn it out.
The -ve and +ve terminals of the Photoelectric Sensor should be connected to 3v3 and GND of the ESP32 and a read wire should go from the ground to the third GPIO pin on the ESP32C3.
Do not forget to add a resistor between the GND and the +ve wire of the Photoelectric Sensor. Otherwise it could burn out.

## Usage
Use the send file on the raspberry pi to send the morse code data  
`sudo ./send [How many times message should be sent] "[Message]"`  
Example `sudo ./send 5 "The quick brown fox jumps over the lazy dog"`   

Then on the ESP32C3 flash the lab5_3 files  
`idf.py flash monitor -p /dev/ttyACM0`  

You should recieve a bunch of messages in colored in Green letter-by-letter
