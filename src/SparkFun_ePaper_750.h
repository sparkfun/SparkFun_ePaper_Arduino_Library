#ifndef SPARKFUN_EPAPER_750_H
#define SPARKFUN_EPAPER_750_H
#include <arduino.h>
#include "SparkFun_ePaper.h"

class EPAPER_750 : public EPAPER
{
    // user-accessible "public" interface
  public:
    //constructor
	EPAPER_750();
	
	//power on and set ePaper display's registers
    //replaces powerOn from base class EPAPER
    void powerOn(void);

  private:

};


#endif
