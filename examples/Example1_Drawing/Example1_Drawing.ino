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
  VCC    Logic Level (if using 5V logic (e.g. Arduino) connect to 5V. if using 3.3V logic (e.g. Teensy) connect to 3.3V)

*/

//Click here to get the library: http://librarymanager/All#SparkFun_ePaper
#include "SparkFun_ePaper_154.h"
#include "SparkFun_ePaper_420.h"

//You must also have the SparkFun HyperDisplay library
//Click here to get the library: http://librarymanager/All#SparkFun_HyperDisplay
//#include "hyperdisplay.h"

#include <SPI.h>
#include <SD.h>

const byte busyPin = 9;
const byte resetPin = 8;
const byte sdCSPin = 7;
const byte srCSPin = 6;
const byte dCSPin = 5;
const byte dcPin = 4;

//Uncomment your display size
EPAPER_154 myEPaper;
//EPAPER_420 myEPaper;

void setup() {
  Serial.begin(9600);
  if (myEPaper.begin(busyPin, resetPin, sdCSPin, srCSPin, dCSPin, dcPin) == false)
    Serial.println("No SD Card Detected");

  //fill display with white
  myEPaper.fillScreen(WHITE);

  //draw red circle centered at (120,0) with radius 35
  myEPaper.circle(120, 0, 35, RED);
  //draw filled white rectangle from (0,0) to (151,25)
  myEPaper.rectangle(0, 0, 151, 25, WHITE, true);
  //draw red circle centered at (130,20) with radius 2
  myEPaper.circle(130, 20, 2, RED);
  //draw red circle centered at (110,20) with radius 2
  myEPaper.circle(110, 20, 2, RED);

  //draw filled red rectangle from (17,25) to (55,62)
  myEPaper.rectangle(17, 25, 55, 62, RED, true);
  //draw black rectangle from (17,25) to (55,62)
  myEPaper.rectangle(17, 25, 55, 62, RED);
  //create arrays of points to connect
  long polygonXPoints1[3] = { 17,  36,  55};
  long polygonYPoints1[3] = { 25,   9,  25};
  //draw black triangle with above points
  myEPaper.polygon(polygonXPoints1, polygonYPoints1, 3, BLACK);
  //draw white rectangle from (43,61) to (53,44)
  myEPaper.rectangle(43, 61, 53, 44, WHITE);

  //draw white filled circle centered at (28,35) with radius 4
  myEPaper.circle(28, 35, 4, WHITE, true);
  //draw black circle centered at (28,35) with radius 7
  myEPaper.circle(28, 35, 7, BLACK);

  //draw line from (4,127) to (31,79)
  myEPaper.line(4, 127, 31, 79, RED);

  myEPaper.updateDisplay();

  //power off the display when done refreshing to prevent damage
  //follow with powerOn to refresh display again
  myEPaper.powerOff();
  
  delay(1000);
  
  //use power on to wake display after begin has already run
  myEPaper.powerOn();
   
  //fill display with white
  myEPaper.fillScreen(WHITE);
  
  //create arrays of points to connect
  long polygonXPoints[5] = { 74, 107,  21, 127,  41};
  long polygonYPoints[5] = { 29, 129,  68,  68, 129};
  //connects the arrays of points in the order given
  myEPaper.polygon(polygonXPoints, polygonYPoints, 5, BLACK);

  //Print "SparkFun Electronics"
  myEPaper.print("SparkFun Electronics");

  //Move start of text to (x,y)=(30,138)
  myEPaper.setTextCursor(30, 138);
  myEPaper.print("Start Something");

  //Drawing circles off screen works too!
  myEPaper.circle(160, -2, 20, RED);
  myEPaper.circle(160, 160, 45, RED);

  //update display
  //the drawings are not seen until this is called
  myEPaper.updateDisplay();
  
  //power off the display when done refreshing to prevent damage
  //follow with powerOn to refresh display again
  myEPaper.powerOff();

}

void loop() {

}
