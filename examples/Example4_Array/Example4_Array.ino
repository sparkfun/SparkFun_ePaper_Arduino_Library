#include "SparkFun_eInk_154.h"
#include <SPI.h>
#include <SD.h>

#include "image.h"

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

  uint16_t n = 0;
  for (uint8_t i = 0; i < 152; i++) {
    uint8_t localBW[19], localR[19];
    for (uint8_t j = 0; j < 19; j++) {
      localBW[j] = pgm_read_byte_near(bwData + n);
      localR[j] = pgm_read_byte_near(rData + n);
      n++;
    }
    myEInk.lineFromArray(0, i, 19, localBW, localR, false);
  }
  myEInk.updateDisplay();


}

void loop() {
  // put your main code here, to run repeatedly:

}

