/*
  This is a library for the ePaper Display with SRAM and micro SD
  By: Ciara Jekel
  SparkFun Electronics
  Date: August 13th, 2018
 
  License: This code is public domain but you buy me a beer if you use this and we meet someday (Beerware license).
 
  Feel like supporting our work? Buy a board from SparkFun!
  https://www.sparkfun.com/products/14892

  Copyright (C) Waveshare     August 7 2017

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documnetation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
  copies of the Software, and to permit persons to  whom the Software is
  furished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS OR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.
*/
#include "SparkFun_ePaper_750.h"

//Constructor
EPAPER_750::EPAPER_750() : EPAPER(152, 152) {
  lineLength = xExt / 8;
  sizeBytes = xExt * yExt / 8;
  addressBW = 0;
  addressR = addressBW + sizeBytes;
}

//power on and set ePaper display's registers
//replaces powerOn from base class EPAPER
void EPAPER_750::powerOn(void) {
  reset();
  sendCommand(BOOSTER_SOFT_START);
  sendData(0x17);
  sendData(0x17);
  sendData(0x17);
  sendCommand(POWER_SETTING);
  sendData(0x07);
  sendData(0x00);
  sendData(0x08);
  sendData(0x00);
  sendCommand(POWER_ON);
  delayWhileBusy();
  sendCommand(PANEL_SETTING);
  sendData(0x0f);
  sendData(0x0d);
  sendCommand(VCOM_AND_DATA_INTERVAL_SETTING);
  sendData(0xF7);
  sendCommand(TCON_RESOLUTION);
  sendData(0x98);
  sendData(0x00);
  sendData(0x98);
  sendCommand(VCM_DC_SETTING_REGISTER);
  sendData(0xf7);
  setLutBw();
  setLutRed();
}

//refresh display after pushing data from SRAM
//if wait is true, will delay until display is idle
void EPAPER_750::updateDisplay(bool wait) {
  _spi->beginTransaction(SPISettings(spiFreq, MSBFIRST, SPI_MODE0));
  _spi->transfer(0x00); //just in case clock idle changed, ensures clk idles in correct position
  delayWhileBusy();
  uint8_t lineBW[lineLength], lineR[lineLength];
  sendCommand(DATA_START_TRANSMISSION_1);
  for (uint16_t i = 0; i < yExt; i++) {
    readSRAM(addressBW + (lineLength * i), line, lineLength);
    
    _sendBWR(lineBW, lineR, lineLength);
  }
  
  sendCommand(DISPLAY_REFRESH);
  if (wait)
    delayWhileBusy();
  _spi->endTransaction();
}

void EPAPER_750::_sendBWR(uint8_t dataBW[], uint8_t dataR[], uint16_t bytesToSend) {
  digitalWrite(_dcPin, HIGH); //data
  digitalWrite(_dCSPin, LOW);
  for (uint16_t i = 0; i < bytesToSend; i++) {
    _spi->transfer(data[i]);
  }
  digitalWrite(_dCSPin, HIGH);
}