/*
  ePaper Display with SRAM and micro SD
  By: Ciara Jekel
  SparkFun Electronics
  Date: August 13th, 2018
  License: This code is public domain but you buy me a beer if you use this and we meet someday (Beerware license).

  Feel like supporting our work? Buy a board from SparkFun!
  https://www.sparkfun.com/products/14892

  This example draws basic shapes on the display.

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

  //fill display with red
  myEPaper.fillScreen(WHITE);
  myEPaper.circle(120, 0, 35, RED);
  myEPaper.rectangle(0, 0, 151, 25, WHITE, true);
  myEPaper.circle(130, 20, 2, RED);
  myEPaper.circle(110, 20, 2, RED);

  myEPaper.rectangle(17, 25, 55, 62, RED, true);
  myEPaper.rectangle(17, 25, 55, 62, BLACK);
  long polygonXPoints1[3] = { 17,  36,  55};
  long polygonYPoints1[3] = { 25,   9,  25};
  myEPaper.polygon(polygonXPoints1, polygonYPoints1, 3, BLACK);
  myEPaper.rectangle(43, 61, 53, 44, WHITE);
  myEPaper.circle(28, 35, 4, WHITE, true);
  myEPaper.circle(28, 35, 7, BLACK);


  //  myEPaper.print("hey");
  //  //draw black rectangle
  //  myEPaper.rectangle(12, 12, 110, 110, BLACK);
  //  //draw white rectangle
  //  myEPaper.rectangle(14, 14, 108, 108, WHITE);
  //  //draw black filled rectangle
  //  myEPaper.rectangle(16, 16, 106, 106, BLACK, true);
  //
  //  myEPaper.circle(60, 50, 29, RED, true);
  //  myEPaper.circle(60, 50, 20, WHITE);
  //  myEPaper.line(60, 50, 24, 133, WHITE);
  //  myEPaper.line(50, 24, 133, 60, WHITE);
  //  myEPaper.line(133, 24, 60, 50, WHITE);
  //  myEPaper.line(133, 60, 24, 50, WHITE);
  //  myEPaper.line(100, 150, 130, 130, WHITE);
  //  myEPaper.line(100, 150, 100, 100, WHITE);

  myEPaper.line(4, 127, 31, 79, RED);

  long polygonXPoints[5] = { 74, 107,  21, 127,  41};
  long polygonYPoints[5] = { 29, 129,  68,  68, 129};
  myEPaper.polygon(polygonXPoints, polygonYPoints, 5, BLACK);

  //update display
  //the drawings are not seen until this is called
  myEPaper.updateDisplay();

}

void loop() {
  // put your main code here, to run repeatedly:

}

