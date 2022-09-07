Please wait, it's an update of W25Q16 library

in build.. wait please

Arduino library for the Winbond W25Q128 Serial Flash.

## Connections for W25Q128 to Arduno Uno

VCC ->	to arduino 3.3v
CS ->	to arduino pin 10
DOut ->	to arduino pin 12
GND ->	to arduino GND
CLK ->	to arduino pin 13
DIn ->	to arduino pin 11

NOTE: The Winbond W25Q128 requires 2.7V to 3.6V for operation.  The Arduino Uno operates at 5V so I had converted an Arduino Uno from 5V to 3.3V to utilize with the W25Q128.  I used the technique described at https://learn.adafruit.com/arduino-tips-tricks-and-techniques/3-3v-conversion to convert an Arduino Uno to 3.3V.

Plages mémoire	0 à 65535
Adresses mémoire	0 à 255
Valeurs stockable	octet (0 à 255)
