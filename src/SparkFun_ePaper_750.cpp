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
EPAPER_750::EPAPER_750() : EPAPER(640, 385) {
  lineLength = xExt / 8;
  sizeBytes = (long) xExt * (long) yExt / 8;
  addressBW = 0;
  addressR = addressBW + sizeBytes;
}

//power on and set ePaper display's registers
//replaces powerOn from base class EPAPER
void EPAPER_750::powerOn(void) {
  reset();

  
  sendCommand(BOOSTER_SOFT_START);
  sendData(0xc7);
  sendData(0xcc);
  sendData(0x28);

  sendCommand(POWER_SETTING);
  sendData(0x37);
  sendData(0x00);
  sendData(0x08);
  sendData(0x08);

  sendCommand(POWER_ON);
  delayWhileBusy();
  
  sendCommand(PANEL_SETTING);
  sendData(0xCF);
  sendData(0x00);
  
  sendCommand(PLL_CONTROL);
  sendData(0x3A);
  
  sendCommand(TCON_RESOLUTION);
  sendData(0x02);     //source 640
  sendData(0x80);
  sendData(0x01);     //gate 384
  sendData(0x80);
  
  sendCommand(VCM_DC_SETTING_REGISTER);
  sendData(0x1e);
  
  sendCommand(VCOM_AND_DATA_INTERVAL_SETTING);
  sendData(0x17);
  
  sendCommand(0xe5);
  sendData(0x03);  
}

//refresh display after pushing data from SRAM
//if wait is true, will delay until display is idle
//if wait is false, make sure to call flash() (after checking busy)
void EPAPER_750::updateDisplay(bool wait) {
  _spi->beginTransaction(SPISettings(spiFreq, MSBFIRST, SPI_MODE0));
  _spi->transfer(0x00); //just in case clock idle changed, ensures clk idles in correct position
  delayWhileBusy();
  uint8_t lineBW[lineLength], lineR[lineLength];
  sendCommand(DATA_START_TRANSMISSION_1);
  for (uint16_t i = 0; i < yExt; i++) {
    EPAPER::readSRAM(addressBW + (lineLength * i), lineBW, lineLength);
    EPAPER::readSRAM(addressR + (lineLength * i), lineR, lineLength);
    _sendBWR(lineBW, lineR, lineLength);
    //    _sendBWR(lineBW, lineR, lineLength);
  }

  sendCommand(DISPLAY_REFRESH);
  
  //passing a 'false' for wait might make the display fail to refresh... 
  //unless you call flash after, then it's fine. proceed with caution
  if (wait)
    delayWhileBusy();
	
  //enable flash then disable flash
  flash();
  
  _spi->endTransaction();
}
int n = 0; //rmv

void EPAPER_750::_sendBWR(uint8_t dataBW[], uint8_t dataR[], uint16_t bytesToSend) {
  digitalWrite(_dcPin, HIGH); //data
  digitalWrite(_dCSPin, LOW);
  uint8_t outData;
  for (uint16_t i = 0; i < bytesToSend; i++)
  {
    for (int8_t j = 7; j  > 0 ; j -= 2)
    {
      outData = 0;
      if ((dataBW[i] & (1 << (j - 1))) != 0)
        outData |= 0x03;  //White
      else if ((dataR[i] & (1 << (j - 1))) != 0) {
        outData &= 0x00;  //Black
      }
      else
        outData |= 0x04;
      if ((dataBW[i] & (1 << (j - 0))) != 0)
        outData |= 0x30;  //White
      else if ((dataR[i] & (1 << (j - 0))) != 0) {
        outData &= 0x0F;  //Black
      }
      else
        outData |= 0x40;
      sendData(outData);
    }
  }
  digitalWrite(_dCSPin, HIGH);
}

void EPAPER_750::flash(void){
  sendCommand(0x65);
  sendData(0x01);
  sendCommand(0xb9);
  sendCommand(0x65);
  sendData(0x00);
}
/*
  About the image data: 4 bits = 1 pixel, doesn’t support Gray scale
  (can only display black and white).
  0b0000 stands for a black pixel,
  0b0011 stands for a white pixel,
  0b0100 stands for a red pixel.
*/
