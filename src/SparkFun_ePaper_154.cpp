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
#include "SparkFun_ePaper_154.h"

//Constructor
EPAPER_154::EPAPER_154() {
  xExt = 152;
  yExt = 152;
  lineLength = xExt / 8;
  sizeBytes = xExt * yExt / 8;
  addressBW = 0;
  addressR = addressBW + sizeBytes;

  unsigned char lut_vcom0[15] =
  {
    0x0E, 0x14, 0x01, 0x0A, 0x06, 0x04, 0x0A, 0x0A,
    0x0F, 0x03, 0x03, 0x0C, 0x06, 0x0A, 0x00
  };

  unsigned char lut_w[15] =
  {
    0x0E, 0x14, 0x01, 0x0A, 0x46, 0x04, 0x8A, 0x4A,
    0x0F, 0x83, 0x43, 0x0C, 0x86, 0x0A, 0x04
  };

  unsigned char lut_b[15] =
  {
    0x0E, 0x14, 0x01, 0x8A, 0x06, 0x04, 0x8A, 0x4A,
    0x0F, 0x83, 0x43, 0x0C, 0x06, 0x4A, 0x04
  };

  unsigned char lut_g1[15] =
  {
    0x8E, 0x94, 0x01, 0x8A, 0x06, 0x04, 0x8A, 0x4A,
    0x0F, 0x83, 0x43, 0x0C, 0x06, 0x0A, 0x04
  };

  unsigned char lut_g2[15] =
  {
    0x8E, 0x94, 0x01, 0x8A, 0x06, 0x04, 0x8A, 0x4A,
    0x0F, 0x83, 0x43, 0x0C, 0x06, 0x0A, 0x04
  };

  unsigned char lut_vcom1[15] =
  {
    0x03, 0x1D, 0x01, 0x01, 0x08, 0x23, 0x37, 0x37,
    0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
  };

  unsigned char lut_red0[15] =
  {
    0x83, 0x5D, 0x01, 0x81, 0x48, 0x23, 0x77, 0x77,
    0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
  };

  unsigned char lut_red1[15] =
  {
    0x03, 0x1D, 0x01, 0x01, 0x08, 0x23, 0x37, 0x37,
    0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
  };
}

void EPAPER_154::powerOn(void) {
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
  SetLutBw();
  SetLutRed();
}

