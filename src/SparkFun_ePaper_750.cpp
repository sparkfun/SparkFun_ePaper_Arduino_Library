/*
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
EPAPER_750::EPAPER_750() : EPAPER(640, 385){
  lineLength = xExt / 8;
  sizeBytes = xExt * yExt / 8;
  addressBW = 0;
  addressR = addressBW + sizeBytes;

}

//power on and set ePaper display's registers
//replaces powerOn from base class EPAPER
void EPAPER_750::powerOn(void) {
//  reset();
 // sendCommand(BOOSTER_SOFT_START);
  //sendData(0x17);
  //sendData(0x17);
  //sendData(0x17);
//  sendCommand(POWER_SETTING);
//  sendData(0x03);
//  sendData(0x00);
//  sendData(0x2b); 
//  sendData(0x2b);
//  sendData(0x09);
//  sendCommand(POWER_ON);
  //de/layWhileBusy();
  //sendCommand(PANEL_SETTING);
  ////sendData(0x0f);
  //sendCommand(TCON_RESOLUTION);
//  sendData(0x01);
  //sendData(0x90);
  //sendData(0x01);
  //sendData(0x2c);
//  sendCommand(VCOM_AND_DATA_INTERVAL_SETTING);
//  sendData(0x7);

  //PLL_CONTROL
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
  sendData(0xcf);
  sendData(0x80);
  sendCommand(TCON_RESOLUTION);
  sendData(0x02);
  sendData(0x80);
  sendData(0x01);
  sendData(0x80);
  sendCommand(VCM_DC_SETTING_REGISTER);
  sendData(0x1e);
  sendCommand(VCOM_AND_DATA_INTERVAL_SETTING);
  sendData(0x17);


  setLutBw();
  setLutRed();
}

