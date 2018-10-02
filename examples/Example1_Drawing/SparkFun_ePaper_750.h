/*
  This is a library for the ePaper Display with SRAM and micro SD
  By: Ciara Jekel
  SparkFun Electronics
  Date: August 13th, 2018

  License: This code is public domain but you buy me a beer if you use this and we meet someday (Beerware license).

  Feel like supporting our work? Buy a board from SparkFun!
  https://www.sparkfun.com/products/14892
*/
#ifndef SPARKFUN_EPAPER_750_H
#define SPARKFUN_EPAPER_750_H
#include <arduino.h>
#include "SparkFun_ePaper.h"

class EPAPER_750: public EPAPER {
   
    // user-accessible "public" interface
  public:
    //constructor
    EPAPER_750();

    //power on and set ePaper display's registers
    //replaces powerOn from base class EPAPER
    void powerOn(void);
    //refresh display after pushing data from SRAM
    //if wait is true, will delay until display is idle
    void updateDisplay(bool wait = true);
    void _sendBWR(uint8_t dataBW[], uint8_t dataR[], uint16_t bytesToSend);
  private:

};


#endif
