/*
  This is a library for the ePaper Display with SRAM and micro SD
  By: Ciara Jekel
  SparkFun Electronics
  Date: August 13th, 2018
 
  License: This code is public domain but you buy me a beer if you use this and we meet someday (Beerware license).
 
  Feel like supporting our work? Buy a board from SparkFun!
  https://www.sparkfun.com/products/14892
*/
#ifndef SPARKFUN_EPAPER_420_H
#define SPARKFUN_EPAPER_420_H
#include <arduino.h>
#include "SparkFun_ePaper.h"

class EPAPER_420 : public EPAPER
{
    // user-accessible "public" interface
  public:
	//constructor
    EPAPER_420();
	
	//power on and set ePaper display's registers
    //replaces powerOn from base class EPAPER
    void powerOn(void);

  private:

};


#endif
