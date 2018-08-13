/*
  This is a library for the ePaper Display with SRAM and micro SD
  By: Ciara Jekel
  SparkFun Electronics
  Date: August 13th, 2018
  License: This code is public domain but you buy me a beer if you use this and we meet someday (Beerware license).

  Feel like supporting our work? Buy a board from SparkFun!
  https://www.sparkfun.com/products/14892

*/

#include "SparkFun_ePaper.h"
#define MAX(a,b) {a>b?a:b;}
#define MIN(a,b) {a>b?b:a;}
#define SWAP(a,b) {uint16_t t; a<b?:(t=a, a=b, b=t);}

File ePaperFile;

void EPAPER::reset(void) {
  digitalWrite(_resetPin, LOW);
  delay(200);
  digitalWrite(_resetPin, HIGH);
  delay(200);
}

bool EPAPER::begin(uint8_t busyPin, uint8_t resetPin, uint8_t sdCSPin, uint8_t srCSPin, uint8_t dCSPin, uint8_t dcPin) {
  bool sdCard = true;
  SPI.begin();
  SPI.beginTransaction(SPISettings(2000000, MSBFIRST, SPI_MODE0));

  _busyPin = busyPin;
  _resetPin = resetPin;
  _sdCSPin = sdCSPin;
  _srCSPin = srCSPin;
  _dCSPin = dCSPin;
  _dcPin = dcPin;

  pinMode(_busyPin, INPUT);
  pinMode(_resetPin, OUTPUT);
  pinMode(_sdCSPin, OUTPUT);
  pinMode(_srCSPin, OUTPUT);
  pinMode(_dCSPin, OUTPUT);
  pinMode(_dcPin, OUTPUT);

  beginSRAM();
  if (!SD.begin(sdCSPin))
    sdCard = false;
  powerOn();
  return sdCard;
}

void EPAPER::powerOn(void) {
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

void EPAPER::powerOff(void) {
  sendCommand(POWER_OFF);
  sendCommand(DEEP_SLEEP);
  sendData(0xA5);
}

void EPAPER::sendBW(uint8_t data[], uint16_t bytesToSend) {
  sendCommand(DATA_START_TRANSMISSION_1);
  _sendBW(data, bytesToSend);
}

void EPAPER::sendR(uint8_t data[], uint16_t bytesToSend) {
  sendCommand(DATA_START_TRANSMISSION_2);
  _sendR(data, bytesToSend);
}

void EPAPER::refreshDisplay(bool wait) {
  delayWhileBusy();
  sendCommand(DISPLAY_REFRESH);
  delayWhileBusy();
  if (wait)
    delayWhileBusy();

}

//updates from SRAM
void EPAPER::updateDisplay(bool wait) {
  delayWhileBusy();
  uint8_t line[lineLength];
  sendCommand(DATA_START_TRANSMISSION_1);
  for (uint16_t i = 0; i < yExt; i++) {
    readSRAM(addressBW + (lineLength * i), line, lineLength);
    _sendBW(line, lineLength);
  }
  sendCommand(DATA_START_TRANSMISSION_2);
  for (uint16_t i = 0; i < yExt; i++) {
    readSRAM(addressR + (lineLength * i), line, lineLength);
    _sendR(line, lineLength);

  }
  sendCommand(DISPLAY_REFRESH);
  if (wait)
    delayWhileBusy();

}

void EPAPER::fillFromArray(epaper_color_t data[], uint16_t arrLen, bool update, bool wait) {
  uint8_t bwData[lineLength], rData[lineLength];
  uint16_t y, n = 0;
  for (y = 0; y < yExt; y++) {
    uint16_t j = 0;
    for (uint16_t i = 0; i < xExt; i++) {
      uint16_t ind = arrLen == 0 ? n : n % arrLen;
      if (data[ind] == WHITE) { //white
        bwData[j] |= (1 << (7 - (i % 8))); //1
        rData[j] |= (1 << (7 - (i % 8))); //1
      }
      else if (data[ind] == RED) { //red
        bwData[j] &= ~(1 << (7 - (i % 8))); //0
        rData[j] &= ~(1 << (7 - (i % 8))); //0
      }
      else { //black
        bwData[j]  &= ~(1 << (7 - (i % 8))); //0
        rData[j]  |= (1 << (7 - (i % 8))); //1
      }
      n++;
      if (n % 8 == 0) {
        j++;
      }
    }
    lineFromArray(0, y, lineLength, bwData, rData, false);
  }
  if (update)
    updateDisplay(wait);
}

void EPAPER::lineFromArray(uint16_t x, uint16_t y, uint16_t pixels, epaper_color_t data[], bool update, bool wait) {
  if (x + pixels > xExt) pixels = xExt - x;
  if (y > yExt) return;
  uint8_t arrLen = (x + pixels - 1) / 8 - (x / 8) + 1;
  uint8_t bwData[arrLen], rData[arrLen];
  uint16_t address = lineLength * y + x / 8;
  readSRAM (address, bwData, arrLen);  //initialize to make sure nothing is overwritten
  readSRAM (address + sizeBytes, rData, arrLen);  //initialize to make sure nothing is overwritten
  uint16_t j = 0, n = x;
  for (uint16_t i = 0; i < pixels; i++) {
    if (data[i] == WHITE) { //white
      bwData[j] |= (1 << (7 - (n % 8))); //1
      rData[j] |= (1 << (7 - (n % 8))); //1
    }
    else if (data[i] == RED) { //red
      bwData[j] &= ~(1 << (7 - (n % 8))); //0
      rData[j] &= ~(1 << (7 - (n % 8))); //0
    }
    else { //black
      bwData[j]  &= ~(1 << (7 - (n % 8))); //0
      rData[j]  |= (1 << (7 - (n % 8))); //1
    }
    n++;
    if (n % 8 == 0) {
      j++;
    }
  }
  lineFromArray(x, y, arrLen, bwData, rData, update, wait);
}

//if len=0 array is size of screen (xExt*yExt)
void EPAPER::fillFromArray(uint8_t bwData[], uint8_t rData[], uint16_t arrLen, bool update, bool wait) {
  if (arrLen == 0) arrLen = sizeBytes;
  int16_t i;
  uint16_t j = 0;
  for ( i = sizeBytes; i > 0; i -= arrLen) {
    writeSRAM(addressBW + arrLen * j, bwData, arrLen);
    writeSRAM(addressR + arrLen * j, rData, arrLen);
    j++;
  }
  writeSRAM(addressBW + arrLen * j, bwData, i);
  writeSRAM(addressR + arrLen * j, rData, i);
  if (update)
    updateDisplay(wait);
}

void EPAPER::lineFromArray(uint16_t x, uint16_t y, uint16_t bytes, uint8_t bwData[], uint8_t rData[], bool update, bool wait) {
  uint16_t address = lineLength * y + x / 8;
  writeSRAM(address, bwData, bytes);
  writeSRAM(address + sizeBytes, rData, bytes);
  if (update)
    updateDisplay(wait);
}

void EPAPER::xline(uint16_t x, uint16_t y, uint16_t Len, epaper_color_t color) {
  if (x + Len > xExt) Len = xExt - x;
  if (y > yExt) return;
  uint8_t arrLen = (x + Len - 1) / 8 - (x / 8) + 1;
  uint8_t bwData[arrLen], rData[arrLen]; //Need to reserve space considering case of overlaps (e.g. 3 bytes can be covered by 10 bits)
  uint16_t address = lineLength * y + x / 8;
  readSRAM (addressBW + address, bwData, arrLen); //initialize array to make sure nothing is overwritten
  readSRAM (addressR + address, rData, arrLen); //initialize array to make sure nothing is overwritten
  if (color == WHITE) { //white
    uint16_t j = 0;
    for (uint16_t i = 0; i < Len; i++)
    {
      bwData[j] |= (1 << (7 - (x % 8))); // 1
      rData[j] |= (1 << (7 - (x % 8)));  // 1
      x++;
      if (x % 8 == 0)
        j++;
    }
  }
  else if (color == RED) { //red
    uint16_t j = 0;
    for (uint16_t i = 0; i < Len; i++)
    {
      bwData[j] &= ~(1 << (7 - (x % 8))); // 0
      rData[j] &= ~(1 << (7 - (x % 8)));  // 0
      x++;
      if (x % 8 == 0)
        j++;
    }
  }
  else { //black
    uint16_t j = 0;
    for (uint16_t i = 0; i < Len; i++)
    {
      bwData[j]  &= ~(1 << (7 - (x % 8))); // 0
      rData[j]  |= (1 << (7 - (x % 8)));   // 1
      x++;
      if (x % 8 == 0)
        j++;
    }
  }
  writeSRAM(addressBW + address, bwData, arrLen);
  writeSRAM(addressR + address, rData, arrLen);
}
void EPAPER::yline(uint16_t x, uint16_t y, uint16_t Len, epaper_color_t color) {
  for (uint16_t i = 0; i < Len; i++) {
    pixel(x, y + i, color);
  }
}

void EPAPER::pixel(uint16_t x, uint16_t y, epaper_color_t color) {
  if (y > yExt || x > xExt) return;
  uint8_t bwData, rData;
  uint16_t address = lineLength * y + x / 8;
  readSRAM (addressBW + address, &bwData, 1);  //initialize to make sure nothing is overwritten
  readSRAM (addressR + address, &rData, 1);  //initialize to make sure nothing is overwritten
  if (color == WHITE) { //white
    bwData |= (1 << (7 - (x % 8))); //1
    rData |= (1 << (7 - (x % 8))); //1
  }
  else if (color == RED) { //red
    bwData &= ~(1 << (7 - (x % 8))); //0
    rData &= ~(1 << (7 - (x % 8))); //0
  }
  else { //black
    bwData  &= ~(1 << (7 - (x % 8))); //0
    rData  |= (1 << (7 - (x % 8))); //1
  }
  writeSRAM(address, &bwData, 1);
  writeSRAM(address + sizeBytes, &rData, 1);
}
void EPAPER::rectangle(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, epaper_color_t color, bool filled = false) {
  SWAP(x0, x1);
  SWAP(y0, y1);
  uint16_t len = x1 - x0 + 1;
  if (filled) {

    for (uint16_t i = y0; i <= y1; i++) {
      xline(x0, i, len, color);
    }
  }
  else {
    uint16_t ylen = y1 - y0 + 1;
    xline(x0, y0, len, color);
    xline(x0, y1, len, color);
    yline(x0, y0, ylen, color);
    yline(x1, y0, ylen, color);
  }
}

void EPAPER::fillScreen(epaper_color_t color) {
  rectangle(0, 0, xExt - 1, yExt - 1, color, true);
}

bool EPAPER::bmpFromSD(char * filename, byte whiteMin, byte redMin, bool update, bool wait) {
  ePaperFile = SD.open(filename);
  byte data[4];
  if (!ePaperFile) return false;
  ePaperFile.seek(10);
  ePaperFile.read(data, 4);
  int offSet = (data[0]) + (data[1] << 8) + (data[2] << 16) + (data[3] << 24);
  if (offSet != 54) return false;
  ePaperFile.read(data, 4);
  int headSize = (data[0]) + (data[1] << 8) + (data[2] << 16) + (data[3] << 24);
  if (headSize != 40) return false;
  ePaperFile.read(data, 4);
  int width = (data[0]) + (data[1] << 8) + (data[2] << 16) + (data[3] << 24);
  ePaperFile.read(data, 4);
  int height = (data[0]) + (data[1] << 8) + (data[2] << 16) + (data[3] << 24);
  ePaperFile.seek(28);
  ePaperFile.read(data, 2);
  int bpp = (data[0]) + (data[1] << 8) ;
  if (bpp != 24 ) return false;
  _bmp24(whiteMin, redMin, width, height);
  ePaperFile.close();
  if (update)
    updateDisplay(wait);
  return true;
}

bool EPAPER::saveToSD(char * filename) {
  ePaperFile = SD.open(filename, FILE_WRITE | O_TRUNC); //overwrite 
  if (!ePaperFile) return false;
  ePaperFile.seek(0);
  ePaperFile.write(xExt >> 8);
  ePaperFile.write(xExt);
  ePaperFile.write(yExt >> 8);
  ePaperFile.write(yExt);
  uint8_t dataBW[lineLength];
  uint8_t dataR[lineLength];

  for (uint16_t i = 0; i < yExt; i++) {
    readSRAM(addressBW + (lineLength) * i, dataBW, lineLength);
    readSRAM(addressR + (lineLength) * i, dataR, lineLength);
    ePaperFile.write(dataBW, lineLength);
    ePaperFile.write(dataR, lineLength);
  }
  ePaperFile.close();
  return true;
}
bool EPAPER::loadFromSD(char * filename, bool update, bool wait) {
  ePaperFile = SD.open(filename);
  if (!ePaperFile) return false;
  uint16_t width = (ePaperFile.read() << 8) + ePaperFile.read();
  uint16_t height = (ePaperFile.read() << 8) + ePaperFile.read();
  uint8_t dataBW[lineLength];
  uint8_t dataR[lineLength];
  for (uint16_t i = 0; i < yExt && ePaperFile.available() > 2 * lineLength; i++) {
    ePaperFile.read(dataBW, lineLength);
    ePaperFile.read(dataR, lineLength);
    writeSRAM(addressBW + lineLength * i, dataBW, lineLength);
    writeSRAM(addressR + lineLength * i, dataR, lineLength);
  }
  ePaperFile.close();
  if (update)
    updateDisplay(wait);
  return true;
}

void EPAPER::delayWhileBusy(void) {
  while (digitalRead(_busyPin) == 0) {     //0: busy, 1: idle
    delay(1);
  }
}

void EPAPER::sendData(uint8_t data) {
  digitalWrite(_dcPin, HIGH);
  digitalWrite(_dCSPin, LOW);
  SPI.transfer(data);
  digitalWrite(_dCSPin, HIGH);
}

void EPAPER::sendCommand(uint8_t command) {
  digitalWrite(_dcPin, LOW);
  digitalWrite(_dCSPin, LOW);
  SPI.transfer(command);
  digitalWrite(_dCSPin, HIGH);
}

void EPAPER::SetLutBw(void) {
  uint8_t count;
  sendCommand(0x20);         //g vcom
  for (count = 0; count < 15; count++) {
    sendData(lut_vcom0[count]);
  }
  sendCommand(0x21);        //g ww --
  for (count = 0; count < 15; count++) {
    sendData(lut_w[count]);
  }
  sendCommand(0x22);         //g bw r
  for (count = 0; count < 15; count++) {
    sendData(lut_b[count]);
  }
  sendCommand(0x23);         //g wb w
  for (count = 0; count < 15; count++) {
    sendData(lut_g1[count]);
  }
  sendCommand(0x24);         //g bb b
  for (count = 0; count < 15; count++) {
    sendData(lut_g2[count]);
  }
}

void EPAPER::SetLutRed(void) {
  unsigned int count;
  sendCommand(0x25);
  for (count = 0; count < 15; count++) {
    sendData(lut_vcom1[count]);
  }
  sendCommand(0x26);
  for (count = 0; count < 15; count++) {
    sendData(lut_red0[count]);
  }
  sendCommand(0x27);
  for (count = 0; count < 15; count++) {
    sendData(lut_red1[count]);
  }
}

void EPAPER::_sendBW(uint8_t data[], uint16_t bytesToSend) {
  digitalWrite(_dcPin, HIGH); //data
  digitalWrite(_dCSPin, LOW);
  for (uint16_t i = 0; i < bytesToSend; i++) {
    SPI.transfer(data[i]);
  }
  digitalWrite(_dCSPin, HIGH);
}

void EPAPER::_sendR(uint8_t data[], uint16_t bytesToSend) {
  digitalWrite(_dcPin, HIGH); //data
  digitalWrite(_dCSPin, LOW);
  for (uint16_t i = 0; i < bytesToSend; i++) {
    SPI.transfer(data[i]);
  }
  digitalWrite(_dCSPin, HIGH);
}

void EPAPER::_bmp24(uint8_t whiteMin, uint8_t redMin, uint16_t width, uint16_t height) {
  ePaperFile.seek(54);
  byte bmpBW[lineLength];
  byte bmpR[lineLength];
  byte pixel[3];
  for (uint16_t i = 0; i < height && ePaperFile.available(); i++) {
    for (uint16_t j = 0; j < width && ePaperFile.available(); j++) {
      ePaperFile.read(pixel, 3);
      if (pixel[0] > whiteMin && pixel[1] > whiteMin && pixel[2] > whiteMin) {
        bmpBW[j / 8] |= (1 << (7 - (j % 8)));
        bmpR[j / 8]  |= (1 << (7 - (j % 8)));
      }
      else if (pixel[2] > redMin) {
        bmpBW[j / 8] &= ~(1 << (7 - (j % 8)));
        bmpR[j / 8]  &= ~(1 << (7 - (j % 8)));
      }
      else {
        bmpBW[j / 8] &= ~(1 << (7 - (j % 8)));
        bmpR[j / 8]  |= (1 << (7 - (j % 8)));
      }
    }
    writeSRAM(0 + addressR - ((lineLength) * (i + 1)), bmpBW, lineLength);
    writeSRAM(addressR + addressR - ((lineLength) * (i + 1)), bmpR, lineLength);
  }
}

void EPAPER::beginSRAM(void) {
  digitalWrite(_srCSPin, LOW);
  SPI.transfer(WRSR); //Write status register
  SPI.transfer(0x41); //Sequential mode, disable HOLD
  digitalWrite(_srCSPin, HIGH);
}

//read
void EPAPER::readSRAM(uint16_t address, uint8_t buff[], uint16_t bytesToRead) {
  digitalWrite(_srCSPin, LOW);
  SPI.transfer(READ);
  SPI.transfer(address >> 8); //16 bit, MSB don't care (0x0000 to 0x1FFF)
  SPI.transfer(address); //16 bit, MSB don't care (0x0000 to 0x1FFF)
  for (uint16_t i = 0; i < bytesToRead; i++) {
    buff[i] = SPI.transfer(0);
  }
  digitalWrite(_srCSPin, HIGH);
}

//write
void EPAPER::writeSRAM(uint16_t address,  uint8_t buff[], uint16_t bytesToSend) {
  digitalWrite(_srCSPin, LOW);
  SPI.transfer(WRITE);
  SPI.transfer(address >> 8); //16 bit, MSB don't care (0x0000 to 0x1FFF)
  SPI.transfer(address); //16 bit, MSB don't care (0x0000 to 0x1FFF)
  for (uint16_t i = 0; i < bytesToSend; i++) {
    SPI.transfer(buff[i]);
  }
  digitalWrite(_srCSPin, HIGH);
}

