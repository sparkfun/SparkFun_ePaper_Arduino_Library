/*
  This is a library for the ePaper Display with SRAM and micro SD
  By: Ciara Jekel
  SparkFun Electronics
  Date: August 13th, 2018

  License: This code is public domain but you buy me a beer if you use this and we meet someday (Beerware license).

  Feel like supporting our work? Buy a board from SparkFun!
  https://www.sparkfun.com/products/14892
*/

#ifndef SPARKFUN_EPAPER_H
#define SPARKFUN_EPAPER_H

#include<SD.h>
#include <SPI.h>
#include "hyperdisplay.h"

#define PANEL_SETTING                               0x00
#define POWER_SETTING                               0x01
#define POWER_OFF                                   0x02
#define POWER_OFF_SEQUENCE_SETTING                  0x03
#define POWER_ON                                    0x04
#define POWER_ON_MEASURE                            0x05
#define BOOSTER_SOFT_START                          0x06
#define DEEP_SLEEP                                  0x07
#define DATA_START_TRANSMISSION_1                   0x10
#define DATA_STOP                                   0x11
#define DISPLAY_REFRESH                             0x12
#define DATA_START_TRANSMISSION_2                   0x13
#define IMAGE_PROCESS_COMMAND                       0x13
#define PLL_CONTROL                                 0x30
#define TEMPERATURE_SENSOR_COMMAND                  0x40
#define TEMPERATURE_SENSOR_CALIBRATION              0x41
#define TEMPERATURE_SENSOR_WRITE                    0x42
#define TEMPERATURE_SENSOR_READ                     0x43
#define VCOM_AND_DATA_INTERVAL_SETTING              0x50
#define LOW_POWER_DETECTION                         0x51
#define TCON_SETTING                                0x60
#define TCON_RESOLUTION                             0x61
#define SOURCE_AND_GATE_START_SETTING               0x62
#define GET_STATUS                                  0x71
#define AUTO_MEASURE_VCOM                           0x80
#define VCOM_VALUE                                  0x81
#define VCM_DC_SETTING_REGISTER                     0x82
#define PROGRAM_MODE                                0xA0
#define ACTIVE_PROGRAM                              0xA1
#define READ_OTP_DATA                               0xA2

#define READ   0x03     // SRAM's read command
#define WRITE  0x02   // SRAM's write command
#define RDSR   0x05   // SRAM's read status reg
#define WRSR   0x01    // SRAM's write status reg


typedef enum {
  WHITE = 0,
  RED,
  BLACK
} epaper_color_t;

class EPAPER : public hyperdisplay
{
    // user-accessible "public" interface
  public:
    //constructor
    EPAPER(uint16_t xExt, uint16_t yExt);

    uint16_t lineLength; // number of bytes for one line in x direction

    //set up pins, SPI interface, SRAM, SD card, and power on ePaper display
    //returns false if SD card initialization failed
    //optionally pass a different SPI port to use
    //virtual to allow a derived class to overwrite, for example if a diplay doesn't include SRAM or SD card
    virtual bool begin(uint8_t busyPin, uint8_t resetPin, uint8_t sdCSPin, uint8_t srCSPin, uint8_t dCSPin, uint8_t dcPin, SPIClass &spiInterface = SPI);
    //power on and set ePaper display's registers
    //virtual to allow a derived class to overwrite, each ePaper display driver has different power on sequence
    virtual void powerOn(void);
    //power off ePaper display (deep sleep)
    //virtual to allow a derived class to overwrite, if another ePaper display has different sleep sequence
    virtual void powerOff(void);
    //reset ePaper display
    void reset(void);
    //set maximum SPI frequency (will enforce absolute max of 2000000)
    void setFreq(uint32_t frequency);
    //delay while display is busy
    void delayWhileBusy(void);
    //test if busy. returns true if display is busy, false if display is idle
    bool isBusy(void);


    //send command to ePaper display to recieve black/white data, followed by black/white data
    void sendBW(uint8_t data[], uint16_t bytesToSend);
    //send command to ePaper display to recieve red data, followed by red data
    void sendR(uint8_t data[], uint16_t bytesToSend);
    //refresh display without pushing new data from SRAM
    //if wait is true, will delay until display is idle
    void refreshDisplay(bool wait = true);
    //refresh display after pushing data from SRAM
    //if wait is true, will delay until display is idle
    //virtual to be replaced by displays that send data in different ways, i.e. red and black simultaneously
    virtual void updateDisplay(bool wait = true);

    //converts 24 bit bmp to BW/R data array stored on SRAM
    //whiteMin is the minimum threshold for a color to be converted to white in the output (range 0-255)
    //redMin is the minimum threshold for a color to be converted to red in the output (range 0-255)
    //if update is true, will update display with the freshly added SRAM data
    //if wait (and update) are true, will delay until display is idle
    bool bmpFromSD(char * filename, byte whiteMin = 128, byte redMin = 128, bool update = true, bool wait = true);
    //saves current SRAM data to the SD card as a file named filename (should be .txt)
    bool saveToSD(char * filename);
    //loads file from SD card named filename (should be .txt) data to SRAM
    //if update is true, will update display with the freshly added SRAM data
    //if wait (and update) are true, will delay until display is idle
    bool loadFromSD(char * filename, bool update = true, bool wait = true);


    //fill entire screen with data from array of arrLen.
    //will loop through array, making a pattern if array is shorter than sizeBytes
    //for long arrays, consider using the same function name with bwData and rData arrays
    //here, you don't have to create your own formatted array, but it takes up more memory
    void fillFromArray(epaper_color_t data[], uint16_t arrLen, bool update = true, bool wait = true);
    //draw horizontal line of length pixels starting at x,y traveling in the +x direction (right)
    //data array must have length of pixels
    //for long arrays, consider using the same function name with bwData and rData arrays
    //here, you don't have to create your own formatted array, but it takes up more memory
    void lineFromArray(uint16_t x, uint16_t y, uint16_t pixels, epaper_color_t data[], bool update = true, bool wait = true);
    //fill entire screen with data from array of length arrLen.
    //must manipulate pixels in groups of 8
    //will loop through array, making a pattern if array is shorter than sizeBytes
    //create formatted array with WHITE: bw=1 r=1, RED: bw=0, r=0, BLACK: bw=0, r=1
    //most significant bit is leftmost pixel
    void fillFromArray(uint8_t bwData[], uint8_t rData[], uint16_t arrLen, bool update = true, bool wait = true);
    //fill line with data from array of length bytes.
    //must manipulate pixels in groups of 8; starts at byte containing x,y
    //will create line of (pixel) length bytes*8
    //create formatted array with WHITE: bw=1 r=1, RED: bw=0, r=0, BLACK: bw=0, r=1
    //most significant bit is leftmost pixel
    void lineFromArray(uint16_t x, uint16_t y, uint16_t bytes, uint8_t bwData[], uint8_t rData[], bool update = true, bool wait = true);

    //draw a pixel at x0,y0 (with 0,0 as the top left of the display)
    void pixel(uint16_t x0, uint16_t y0, epaper_color_t color);
    //draw a horizontal line of length len starting at x0,y0 and traveling in +y (right)
    void xline(uint16_t x0, uint16_t y0, uint16_t len, epaper_color_t color);
    //draw a vertical line of length len starting at x0,y0, and traveling in +x (down)
    void yline(uint16_t x, uint16_t y, uint16_t len, epaper_color_t color);
    //draw a rectangle from point x0,y0 to x1,y1, outline if filled == false, solid if filled == true
    void rectangle(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, epaper_color_t color, bool filled = false);
    //fill display with color
    void fillScreen(epaper_color_t color);


    //draw from hyperdisplay functions
    void line(int32_t x0, int32_t y0, int32_t x1, int32_t y1, epaper_color_t color);
    void polygon(int32_t x[], int32_t y[], uint8_t numSides, epaper_color_t color);
    void circle(int32_t x0, int32_t y0, uint16_t radius, epaper_color_t color, bool filled = false);
    void fillWindow(epaper_color_t color);


  protected:
    uint8_t   _busyPin , _resetPin, _sdCSPin, _srCSPin, _dCSPin, _dcPin;
    uint16_t sizeBytes;  // number of bytes required to store pixel data (with 1 bit per pixel)
    uint16_t addressBW;  // starting location for black/white pixel data in SRAM
    uint16_t addressR;   // starting location for red pixel data in SRAM
    SPIClass * _spi;     // stores which SPI port to use
    uint32_t spiFreq;    // maximum SPI freq to use (default 2000000)

    //set pins for command and transfer command via SPI
    void sendCommand(uint8_t command);
    //set pins for data and transfer data via SPI
    void sendData(uint8_t data);
    //set look up table
    void setLutBw(void);
    void setLutRed(void);
    //sends black/white data array of length bytesToSend to display
    //virtual to allow a derived class to overwrite, for example 200x200 pixel ePaper sends two bits for each BW data
    virtual void _sendBW(uint8_t data[], uint16_t bytesToSend);
    //sends black/white data array of length bytesToSend to display
    void _sendR(uint8_t data[], uint16_t bytesToSend);
    void _sendImg(uint8_t data[], uint16_t bytesToSend);

    //handles conversion and reading of 24 bit bmp to usable BW/R data arrays
    void _bmp24(uint8_t whiteMin, uint8_t redMin, uint16_t width, uint16_t height) ;

    //set up SRAM
    void beginSRAM(void);
    //read bytesToRead bytes from SRAM starting at address, stores in buff
    void readSRAM (uint16_t address, uint8_t buff[], uint16_t bytesToRead);
    //write bytesToSend bytes from buff to SRAM starting at address
    void writeSRAM (uint16_t address, uint8_t buff[], uint16_t bytesToSend);

    //protected hyperdisplay functions
    void hwpixel(uint16_t x0, uint16_t y0, color_t data = NULL, uint16_t colorCycleLength = 1, uint16_t startColorOffset = 0); 											// Made a pure virtual function so that derived classes are forced to implement the pixel function
    void hwxline(uint16_t x0, uint16_t y0, uint16_t len, color_t data = NULL, uint16_t colorCycleLength = 1, uint16_t startColorOffset = 0, bool goLeft = false); 			// Default implementation provided, suggested to overwrite
    void hwyline(uint16_t x0, uint16_t y0, uint16_t len, color_t data = NULL, uint16_t colorCycleLength = 1, uint16_t startColorOffset = 0, bool goUp = false); 			// Default implementation provided, suggested to overwrite
    color_t getOffsetColor(color_t base, uint32_t numPixels);//= 0;  									// This pure virtual function is required to get the correct pointer after incrementing by a number of pixels (which could have any amount of data behind them depending on how the color is stored)


};

#endif
