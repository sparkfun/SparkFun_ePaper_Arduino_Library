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

  myEPaper.fillScreen(RED);
  myEPaper.rectangle(12, 12, 110, 110, BLACK);
  myEPaper.rectangle(14, 14, 108, 108, WHITE);
  myEPaper.rectangle(16, 16, 106, 106, BLACK, true);
  myEPaper.updateDisplay();

}

void loop() {
  // put your main code here, to run repeatedly:

}

