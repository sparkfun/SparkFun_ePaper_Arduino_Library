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
