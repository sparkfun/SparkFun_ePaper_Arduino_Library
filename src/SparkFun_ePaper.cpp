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
//constructor
EPAPER::EPAPER(uint16_t xExt, uint16_t yExt) : hyperdisplay(xExt, yExt) {
  spiFreq = 2000000;
}


//set up pins, SPI interface, SRAM, SD card, and power on ePaper display
//returns false if SD card initialization failed
//optionally pass a different SPI port to use
//virtual to allow a derived class to overwrite, for example to support a diplay that doesn't include SRAM or SD card
bool EPAPER::begin(uint8_t busyPin, uint8_t resetPin, uint8_t sdCSPin, uint8_t srCSPin, uint8_t dCSPin, uint8_t dcPin, SPIClass &spiInterface) {
  _spi = &spiInterface;
  _spi->begin();
  _spi->beginTransaction(SPISettings(spiFreq, MSBFIRST, SPI_MODE0));
  _spi->transfer(0x00); //just in case clock idle changed, ensures clk idles in correct position
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
  bool sdCard = true;
  if (!SD.begin(sdCSPin))
    sdCard = false;
  powerOn();

  _spi->endTransaction();
  return sdCard;
}

//power on and set ePaper display's registers
//virtual to allow a derived class to overwrite, each ePaper display driver has different power on sequence
void EPAPER::powerOn(void) {
  _spi->beginTransaction(SPISettings(spiFreq, MSBFIRST, SPI_MODE0));
  _spi->transfer(0x00);  //just in case clock idle changed, ensures clk idles in correct position
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
  _spi->endTransaction();
}
//power off ePaper display (deep sleep)
//virtual to allow a derived class to overwrite, if another ePaper display has different sleep sequence
void EPAPER::powerOff(void) {
  _spi->beginTransaction(SPISettings(spiFreq, MSBFIRST, SPI_MODE0));
  _spi->transfer(0x00);
  sendCommand(POWER_OFF);
  sendCommand(DEEP_SLEEP);
  sendData(0xA5);
  _spi->endTransaction();
}

//reset ePaper display
void EPAPER::reset(void) {
  _spi->beginTransaction(SPISettings(spiFreq, MSBFIRST, SPI_MODE0));
  _spi->transfer(0x00); //just in case clock idle changed, ensures clk idles in correct position
  digitalWrite(_resetPin, LOW);
  delay(200);
  digitalWrite(_resetPin, HIGH);
  delay(200);
  _spi->endTransaction();
}
//set maximum SPI frequency (will enforce absolute max of 2000000)
void  EPAPER::setFreq(uint32_t frequency) {
  if (frequency > 2000000)
    spiFreq = 2000000;
  else
    spiFreq = frequency;
}

//delay while display is busy
void EPAPER::delayWhileBusy(void) {
  while (isBusy()) {
    delay(1);
  }
}

//test if busy. returns true if display is busy, false if display is idle
bool  EPAPER::isBusy(void) {
  if (digitalRead(_busyPin) == 0)  //0: busy, 1: idle
    return true;
  else return false;
}

//send command to ePaper display to recieve black/white data, followed by black/white data
void EPAPER::sendBW(uint8_t data[], uint16_t bytesToSend) {
  _spi->beginTransaction(SPISettings(spiFreq, MSBFIRST, SPI_MODE0));
  _spi->transfer(0x00); //just in case clock idle changed, ensures clk idles in correct position
  sendCommand(DATA_START_TRANSMISSION_1);
  _sendBW(data, bytesToSend);
  _spi->endTransaction();
}

//send command to ePaper display to recieve red data, followed by red data
void EPAPER::sendR(uint8_t data[], uint16_t bytesToSend) {
  _spi->beginTransaction(SPISettings(spiFreq, MSBFIRST, SPI_MODE0));
  _spi->transfer(0x00); //just in case clock idle changed, ensures clk idles in correct position
  sendCommand(DATA_START_TRANSMISSION_2);
  _sendR(data, bytesToSend);
  _spi->endTransaction();
}

//refresh display without pushing new data from SRAM
//if wait is true, will delay until display is idle
void EPAPER::refreshDisplay(bool wait) {
  _spi->beginTransaction(SPISettings(spiFreq, MSBFIRST, SPI_MODE0));
  _spi->transfer(0x00); //just in case clock idle changed, ensures clk idles in correct position
  delayWhileBusy();
  sendCommand(DISPLAY_REFRESH);
  if (wait)
    delayWhileBusy();
  _spi->endTransaction();
}

//refresh display after pushing data from SRAM
//if wait is true, will delay until display is idle
void EPAPER::updateDisplay(bool wait) {
  _spi->beginTransaction(SPISettings(spiFreq, MSBFIRST, SPI_MODE0));
  _spi->transfer(0x00); //just in case clock idle changed, ensures clk idles in correct position
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
  _spi->endTransaction();
}


//converts 24 bit bmp to BW/R data array stored on SRAM
//whiteMin is the minimum threshold for a color to be converted to white in the output (range 0-255)
//redMin is the minimum threshold for a color to be converted to red in the output (range 0-255)
//if update is true, will update display with the freshly added SRAM data
//if wait (and update) are true, will delay until display is idle
bool EPAPER::bmpFromSD(char * filename, byte whiteMin, byte redMin, bool update, bool wait) {
  _spi->beginTransaction(SPISettings(spiFreq, MSBFIRST, SPI_MODE0));
  _spi->transfer(0x00); //just in case clock idle changed, ensures clk idles in correct position
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
  _spi->endTransaction();
  return true;
}

//saves current SRAM data to the SD card as a file named filename (should be .txt)
bool EPAPER::saveToSD(char * filename) {
  _spi->beginTransaction(SPISettings(spiFreq, MSBFIRST, SPI_MODE0));
  _spi->transfer(0x00); //just in case clock idle changed, ensures clk idles in correct position
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
  _spi->endTransaction();
  return true;
}

//loads file from SD card named filename (should be .txt) data to SRAM
//if update is true, will update display with the freshly added SRAM data
//if wait (and update) are true, will delay until display is idle
bool EPAPER::loadFromSD(char * filename, bool update, bool wait) {
  _spi->beginTransaction(SPISettings(spiFreq, MSBFIRST, SPI_MODE0));
  _spi->transfer(0x00); //just in case clock idle changed, ensures clk idles in correct position
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
  _spi->endTransaction();
  return true;
}

//fill entire screen with data from array of arrLen. 
//will loop through array, making a pattern if array is shorter than the total number of pixels 
//for long arrays, look into using the same function with bwData and rData arrays
//here, you don't have to create your own formatted array, but it takes up more memory
void EPAPER::fillFromArray(epaper_color_t data[], uint16_t arrLen, bool update, bool wait) {
	if (arrLen ==0) return;
  _spi->beginTransaction(SPISettings(spiFreq, MSBFIRST, SPI_MODE0));
  _spi->transfer(0x00); //just in case clock idle changed, ensures clk idles in correct position
  uint8_t bwData[lineLength], rData[lineLength];
  uint16_t y, n = 0;
  for (y = 0; y < yExt; y++) {
    uint16_t j = 0;
    for (uint16_t i = 0; i < xExt; i++) {
      uint16_t ind = n % arrLen;
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
  _spi->endTransaction();
}

//draw horizontal line of length pixels starting at x,y traveling in the +x direction (right)
//data array must have length of pixels 
//for long arrays, look into using the same function with bwData and rData arrays
//here, you don't have to create your own formatted array, but it takes up more memory
void EPAPER::lineFromArray(uint16_t x, uint16_t y, uint16_t pixels, epaper_color_t data[], bool update, bool wait) {
  _spi->beginTransaction(SPISettings(spiFreq, MSBFIRST, SPI_MODE0));
  _spi->transfer(0x00); //just in case clock idle changed, ensures clk idles in correct position
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
  _spi->endTransaction();
}

//fill entire screen with data from array of length arrLen. 
//must manipulate pixels in groups of 8
//will loop through array, making a pattern if array is shorter than sizeBytes 
//create formatted array with WHITE: bw=1 r=1, RED: bw=0, r=0, BLACK: bw=0, r=1
//most significant bit is leftmost pixel 
void EPAPER::fillFromArray(uint8_t bwData[], uint8_t rData[], uint16_t arrLen, bool update, bool wait) {
  _spi->beginTransaction(SPISettings(spiFreq, MSBFIRST, SPI_MODE0));
  _spi->transfer(0x00); //just in case clock idle changed, ensures clk idles in correct position
  if (arrLen == 0) return;
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
  _spi->endTransaction();
}

//fill line with data from array of length bytes. 
//must manipulate pixels in groups of 8; starts at byte containing x,y 
//will create line of (pixel) length bytes*8
//create formatted array with WHITE: bw=1 r=1, RED: bw=0, r=0, BLACK: bw=0, r=1
//most significant bit is leftmost pixel 
void EPAPER::lineFromArray(uint16_t x, uint16_t y, uint16_t bytes, uint8_t bwData[], uint8_t rData[], bool update, bool wait) {
  _spi->beginTransaction(SPISettings(spiFreq, MSBFIRST, SPI_MODE0));
  _spi->transfer(0x00); //just in case clock idle changed, ensures clk idles in correct position
  uint16_t address = lineLength * y + x / 8;
  writeSRAM(address, bwData, bytes);
  writeSRAM(address + sizeBytes, rData, bytes);
  if (update)
    updateDisplay(wait);
  _spi->endTransaction();
}


//drawing functions from hyperdisplay
void EPAPER::pixel(uint16_t x0, uint16_t y0, epaper_color_t color) {
  epaper_color_t * cptr = &color;
  hyperdisplay::pixel(x0, y0, (color_t) cptr);
}
void EPAPER::xline(uint16_t x0, uint16_t y0, uint16_t len, epaper_color_t color) {
  epaper_color_t * cptr = &color;
  hyperdisplay::xline(x0, y0, len, (color_t) cptr);
}
void EPAPER::yline(uint16_t x0, uint16_t y0, uint16_t len, epaper_color_t color) {
  epaper_color_t * cptr = &color;
  hyperdisplay::yline(x0, y0, len, (color_t) cptr);
}

void EPAPER::rectangle(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, epaper_color_t color, bool filled) {
  epaper_color_t * cptr = &color;
  hyperdisplay::rectangle(x0, y0, x1, y1, filled, (color_t) cptr);
}

void EPAPER::fillScreen(epaper_color_t color) {
  rectangle(0, 0, xExt - 1, yExt - 1, color, true);
}

void EPAPER::line(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, epaper_color_t color) {
  epaper_color_t * cptr = &color;
  hyperdisplay::line(x0, y0, x1, y1, 1, (color_t) cptr);
}
void EPAPER::polygon(int32_t x[], int32_t y[], uint8_t numSides, epaper_color_t color) {
  epaper_color_t * cptr = &color;
  hyperdisplay::polygon(x, y, numSides, 1, (color_t) cptr);
}

void EPAPER::circle(uint16_t x0, uint16_t y0, uint16_t radius, epaper_color_t color, bool filled) {
  epaper_color_t * cptr = &color;
  hyperdisplay::circle( x0,  y0,  radius, filled, (color_t) cptr); // Fills the entire current window
}
void EPAPER::fillWindow(color_t color) {

}
//TODO:
//void fillFromArray(int32_t x0, int32_t y0, int32_t x1, int32_t y1, uint32_t numPixels, color_t data = NULL); 
//void fillWindow(color_t color);  
//uint16_t line(int32_t x0, int32_t y0, int32_t x1, int32_t y1, uint16_t width = 1, color_t data = NULL, uint16_t colorCycleLength = 1, uint16_t startColorOffset = 0, bool reverseGradient = false); 


//set pins for command and transfer command via SPI
void EPAPER::sendData(uint8_t data) {
  digitalWrite(_dcPin, HIGH);
  digitalWrite(_dCSPin, LOW);
  _spi->transfer(data);
  digitalWrite(_dCSPin, HIGH);
}

//set pins for data and transfer data via SPI
void EPAPER::sendCommand(uint8_t command) {
  digitalWrite(_dcPin, LOW);
  digitalWrite(_dCSPin, LOW);
  _spi->transfer(command);
  digitalWrite(_dCSPin, HIGH);
}

//set look up table
void EPAPER::setLutBw(void) {
  uint8_t lut_vcom0[15] = {0x0E, 0x14, 0x01, 0x0A, 0x06, 0x04,
                           0x0A, 0x0A, 0x0F, 0x03, 0x03, 0x0C, 0x06, 0x0A, 0x00
                          };
  uint8_t lut_w[15] = {0x0E, 0x14, 0x01, 0x0A, 0x46, 0x04,
                       0x8A, 0x4A, 0x0F, 0x83, 0x43, 0x0C, 0x86, 0x0A, 0x04
                      };
  uint8_t lut_b[15] = {0x0E, 0x14, 0x01, 0x8A, 0x06, 0x04,
                       0x8A, 0x4A, 0x0F, 0x83, 0x43, 0x0C, 0x06, 0x4A, 0x04
                      };
  uint8_t lut_g1[15] = {0x8E, 0x94, 0x01, 0x8A, 0x06, 0x04,
                        0x8A, 0x4A, 0x0F, 0x83, 0x43, 0x0C, 0x06, 0x0A, 0x04
                       };
  uint8_t lut_g2[15] = {0x8E, 0x94, 0x01, 0x8A, 0x06, 0x04,
                        0x8A, 0x4A, 0x0F, 0x83, 0x43, 0x0C, 0x06, 0x0A, 0x04
                       };

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

//set look up table
void EPAPER::setLutRed(void) {
  uint8_t lut_vcom1[15] = {0x03, 0x1D, 0x01, 0x01, 0x08, 0x23,
                           0x37, 0x37, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
                          };
  uint8_t lut_red0[15] = {0x83, 0x5D, 0x01, 0x81, 0x48, 0x23,
                          0x77, 0x77, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
                         };
  uint8_t lut_red1[15] = {0x03, 0x1D, 0x01, 0x01, 0x08, 0x23,
                          0x37, 0x37, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
                         };

  uint8_t count;
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

//sends black/white data array of length bytesToSend to display
//virtual to allow a derived class to overwrite, for example 200x200 pixel ePaper sends two bits for each BW data
void EPAPER::_sendBW(uint8_t data[], uint16_t bytesToSend) {
  digitalWrite(_dcPin, HIGH); //data
  digitalWrite(_dCSPin, LOW);
  for (uint16_t i = 0; i < bytesToSend; i++) {
    _spi->transfer(data[i]);
  }
  digitalWrite(_dCSPin, HIGH);
}

//sends black/white data array of length bytesToSend to display
void EPAPER::_sendR(uint8_t data[], uint16_t bytesToSend) {
  digitalWrite(_dcPin, HIGH); //data
  digitalWrite(_dCSPin, LOW);
  for (uint16_t i = 0; i < bytesToSend; i++) {
    _spi->transfer(data[i]);
  }
  digitalWrite(_dCSPin, HIGH);
}

//handles conversion and reading of 24 bit bmp to usable BW/R data arrays
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

//set up SRAM
void EPAPER::beginSRAM(void) {
  digitalWrite(_srCSPin, LOW);
  _spi->transfer(WRSR); //Write status register
  _spi->transfer(0x41); //Sequential mode, disable HOLD
  digitalWrite(_srCSPin, HIGH);
}

//read bytesToRead bytes from SRAM starting at address, stores in buff
void EPAPER::readSRAM(uint16_t address, uint8_t buff[], uint16_t bytesToRead) {
  digitalWrite(_srCSPin, LOW);
  _spi->transfer(READ);
  _spi->transfer(address >> 8); //16 bit, MSB don't care (0x0000 to 0x1FFF)
  _spi->transfer(address); //16 bit, MSB don't care (0x0000 to 0x1FFF)
  for (uint16_t i = 0; i < bytesToRead; i++) {
    buff[i] = _spi->transfer(0);
  }
  digitalWrite(_srCSPin, HIGH);
}

//write bytesToSend bytes from buff to SRAM starting at address
void EPAPER::writeSRAM(uint16_t address,  uint8_t buff[], uint16_t bytesToSend) {
  digitalWrite(_srCSPin, LOW);
  _spi->transfer(WRITE);
  _spi->transfer(address >> 8); //16 bit, MSB don't care (0x0000 to 0x1FFF)
  _spi->transfer(address); //16 bit, MSB don't care (0x0000 to 0x1FFF)
  for (uint16_t i = 0; i < bytesToSend; i++) {
    _spi->transfer(buff[i]);
  }
  digitalWrite(_srCSPin, HIGH);
}

void EPAPER::hwpixel(uint16_t x0, uint16_t y0, color_t data, uint16_t colorCycleLength, uint16_t startColorOffset) {
  epaper_color_t ePaperColor = *((epaper_color_t *)data);
  if (y0 > yExt || x0 > xExt) return;
  uint8_t bwData, rData;
  uint16_t address = lineLength * y0 + x0 / 8;
  readSRAM (addressBW + address, &bwData, 1);  //initialize to make sure nothing is overwritten
  readSRAM (addressR + address, &rData, 1);  //initialize to make sure nothing is overwritten
  if (ePaperColor == WHITE) { //white
    bwData |= (1 << (7 - (x0 % 8))); //1
    rData |= (1 << (7 - (x0 % 8))); //1
  }
  else if (ePaperColor == RED) { //red
    bwData &= ~(1 << (7 - (x0 % 8))); //0
    rData &= ~(1 << (7 - (x0 % 8))); //0
  }
  else { //black
    bwData  &= ~(1 << (7 - (x0 % 8))); //0
    rData  |= (1 << (7 - (x0 % 8))); //1
  }
  writeSRAM(address, &bwData, 1);
  writeSRAM(address + sizeBytes, &rData, 1);
}

void EPAPER::hwxline(uint16_t x0, uint16_t y0, uint16_t len, color_t data, uint16_t colorCycleLength, uint16_t startColorOffset, bool goLeft) {
	Serial.println(lineLength);
  epaper_color_t ePaperColor = *((epaper_color_t *)data);
  if (x0 + len > xExt) len = xExt - x0;
  if (y0 > yExt) return;
  uint8_t arrLen = (x0 + len - 1) / 8 - (x0 / 8) + 1;
  uint8_t bwData[arrLen], rData[arrLen]; //Need to reserve space considering case of overlaps (e.g. 3 bytes can be covered by 10 bits)
  uint16_t address = lineLength * y0 + x0 / 8;
  readSRAM (addressBW + address, bwData, arrLen); //initialize array to make sure nothing is overwritten
  readSRAM (addressR + address, rData, arrLen); //initialize array to make sure nothing is overwritten
  if (ePaperColor == WHITE) { //white
    uint16_t j = 0;
    for (uint16_t i = 0; i < len; i++)
    {
      bwData[j] |= (1 << (7 - (x0 % 8))); // 1
      rData[j] |= (1 << (7 - (x0 % 8)));  // 1
      x0++;
      if (x0 % 8 == 0)
        j++;
    }
  }
  else if (ePaperColor == RED) { //red
    uint16_t j = 0;
    for (uint16_t i = 0; i < len; i++)
    {
      bwData[j] &= ~(1 << (7 - (x0 % 8))); // 0
      rData[j] &= ~(1 << (7 - (x0 % 8)));  // 0
      x0++;
      if (x0 % 8 == 0)
        j++;
    }
  }
  else { //black
    uint16_t j = 0;
    for (uint16_t i = 0; i < len; i++)
    {
      bwData[j]  &= ~(1 << (7 - (x0 % 8))); // 0
      rData[j]  |= (1 << (7 - (x0 % 8)));   // 1
      x0++;
      if (x0 % 8 == 0)
        j++;
    }
  }
  writeSRAM(addressBW + address, bwData, arrLen);
  writeSRAM(addressR + address, rData, arrLen);
}
void EPAPER::hwyline(uint16_t x0, uint16_t y0, uint16_t len, color_t data, uint16_t colorCycleLength = 1, uint16_t startColorOffset = 0, bool goUp = false) {
  for (uint16_t i = 0; i < len; i++) {
    hwpixel(x0, y0 + i, data);
  }
}
color_t EPAPER::getOffsetColor(color_t base, uint32_t numPixels){
	return base;
}	
  
  