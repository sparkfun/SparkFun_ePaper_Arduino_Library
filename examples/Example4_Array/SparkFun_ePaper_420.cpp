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
#include "SparkFun_ePaper_420.h"

//Constructor
EPAPER_420::EPAPER_420() : EPAPER(400, 300) {
  lineLength = xExt / 8;
  sizeBytes = (long) xExt * yExt / 8;
  addressBW = 0;
  addressR = addressBW + sizeBytes;
}

//power on and set ePaper display's registers
//replaces powerOn from base class EPAPER
void EPAPER_420::powerOn(void) {
//    reset();
//    sendCommand(BOOSTER_SOFT_START);
//    sendData(0x17);
//    sendData(0x17);
//    sendData(0x17);
//    sendCommand(POWER_ON);
//    delayWhileBusy();
//    sendCommand(PANEL_SETTING);
//    sendData(0x0f);
//    sendCommand(TCON_RESOLUTION);
//    sendData(0x01);
//    sendData(0x90);
//    sendData(0x01);
//    sendData(0x2c);
//    sendCommand(VCOM_AND_DATA_INTERVAL_SETTING);
//    sendData(0x87);

    
//  reset();
//  sendCommand(BOOSTER_SOFT_START);
//  sendData(0x17);
//  sendData(0x17);
//  sendData(0x17);
//  sendCommand(POWER_SETTING);
//  sendData(0x03);
//  sendData(0x00);
//  sendData(0x2b);
//  sendData(0x2b);
//  sendData(0x09);
//  sendCommand(POWER_ON);
//  delayWhileBusy();
//  sendCommand(PANEL_SETTING);
//  sendData(0xaf);
//  sendCommand(PLL_CONTROL);
//  sendData(0x3a);
//  sendCommand(TCON_RESOLUTION);
//  sendData(0x01);
//  sendData(0x90);
//  sendData(0x01);
//  sendData(0x2c);
//  sendCommand(VCM_DC_SETTING_REGISTER);
//  sendData(0x12);
//  sendCommand(VCOM_AND_DATA_INTERVAL_SETTING);
//  sendData(0x87);
//  setLutBw();
//  setLutRed();

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


//  sendCommand(BOOSTER_SOFT_START);
//  sendData(0x17);
//  sendData(0x17);
//  sendData(0x17);
//  sendCommand(POWER_SETTING);
//  sendData(0x07);
//  sendData(0x00);
//  sendData(0x08);
//  sendData(0x00);
//  sendCommand(POWER_ON);
//  delayWhileBusy();
//  sendCommand(PANEL_SETTING);
//  sendData(0x0f);
}
