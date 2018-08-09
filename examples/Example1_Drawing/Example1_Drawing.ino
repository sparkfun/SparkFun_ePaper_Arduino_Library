#include "SparkFun_eInk_154.h"
#include <SPI.h>
#include <SD.h>

const byte busyPin = 9;
const byte resetPin = 8;
const byte sdCSPin = 7;
const byte srCSPin = 6;
const byte dCSPin = 5;
const byte dcPin = 4;

EINK_154 myEInk;

void setup() {
  Serial.begin(9600);

  if (!myEInk.begin(busyPin, resetPin, sdCSPin, srCSPin, dCSPin, dcPin))
    Serial.println("No SD Card Detected");

  myEInk.fillScreen(RED);
  myEInk.rectangle(12, 12, 110, 110, BLACK);
  myEInk.rectangle(14, 14, 108, 108, WHITE);
  myEInk.rectangle(16, 16, 106, 106, BLACK, true);
  myEInk.updateDisplay();

}

void loop() {
  // put your main code here, to run repeatedly:

}

