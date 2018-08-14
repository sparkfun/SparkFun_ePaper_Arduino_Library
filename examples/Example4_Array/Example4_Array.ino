/*
  ePaper Display with SRAM and micro SD
  By: Ciara Jekel
  SparkFun Electronics
  Date: August 13th, 2018
  License: This code is public domain but you buy me a beer if you use this and we meet someday (Beerware license).

  Feel like supporting our work? Buy a board from SparkFun!
  https://www.sparkfun.com/products/14892

  This example loads an array of preformatted data from program memory to the display.
  Copy arrays from "array.txt" into image.h from python script output for array from bitmap. (BMPtoEPD)

  Color Mapping:
  Color bwData rData
  WHITE    1     1
  RED      0     0
  BLACK    0     1

  Each pixel takes two bits. The most significant bit corresponds to the first pixel
  Ex:
    bwData = 0b1001011
    rData  = 0b1101011
    pixels :   WBRWRWW

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

*/

#include "SparkFun_ePaper_154.h"
#include <SPI.h>
#include <SD.h>

#include "image.h"

const byte busyPin = 9;
const byte resetPin = 8;
const byte sdCSPin = 7;
const byte srCSPin = 6;
const byte dCSPin = 5;
const byte dcPin = 4;

EPAPER_154 myEPaper;
byte yDimension = 152;

void setup() {
  Serial.begin(9600);

  if (!myEPaper.begin(busyPin, resetPin, sdCSPin, srCSPin, dCSPin, dcPin))
    Serial.println("No SD Card Detected");

  //get data line by line and push each line to the display with lineFromArray
  //could also (in theory, memory issues arise) use fillFromArray to do all at once
  short n = 0;
  //for each line of our display we push a line of pixels
  for (byte i = 0; i < yDimension; i++) {
    uint8_t localBW[19], localR[19];
    //fill array with 19 bytes from PROGMEM
    for (byte j = 0; j < 19; j++) {
      localBW[j] = pgm_read_byte(bwData + n);
      localR[j] = pgm_read_byte(rData + n);
      n++;
    }
    //fill line starting at the byte containing (x,y) = (0,i) with 19 bytes from localBW and localR without updating the display
    myEPaper.lineFromArray(0, i, 19, localBW, localR, false);
  }
  //update the display
  myEPaper.updateDisplay();


}

void loop() {
  // put your main code here, to run repeatedly:

}

