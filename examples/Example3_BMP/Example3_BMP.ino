/*
  ePaper Display with SRAM and micro SD
  By: Ciara Jekel
  SparkFun Electronics
  Date: August 13th, 2018
  License: This code is public domain but you buy me a beer if you use this and we meet someday (Beerware license).

  Feel like supporting our work? Buy a board from SparkFun!
  https://www.sparkfun.com/products/14892

  This example loads a bitmap from the SD card onto the display.

  Hardware Connections:
  BUSY   9
  RST    8
  MISO  12
  MOSI  11
  SCK   13
  SDCS   7
  SRCS   6
  DCS    5 
  D/C    4
  GND    GND
  5V     Logic Level (if using 5V logic (e.g. Arduino) connect to 5V. if using 3.3V logic (e.g. Teensy) connect to 3.3V)
  SD in SD card slot
    Bitmap image (24bit) with same dimensions as display on SD card named "img.bmp"
*/

#include "SparkFun_ePaper_154.h"
#include <SPI.h>
#include <SD.h>

const byte busyPin = 9;
const byte resetPin = 8;
const byte sdCSPin = 7;
const byte srCSPin = 6;
const byte dCSPin = 5;
const byte dcPin = 4;

EPAPER_154 myEPaper;

void setup() {
  Serial.begin(9600);

  if (!myEPaper.begin(busyPin, resetPin, sdCSPin, srCSPin, dCSPin, dcPin))
    Serial.println("No SD Card Detected");

  myEPaper.bmpFromSD("img.bmp", 128, 128);


}

void loop() {
  // put your main code here, to run repeatedly:

}

