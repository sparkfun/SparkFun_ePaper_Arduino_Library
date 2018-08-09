#ifndef SPARKFUN_EINK_H
#define SPARKFUN_EINK_H

#include<SD.h>
#include <SPI.h>

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
} eink_color_t;

class EINK
{
    // user-accessible "public" interface
  public:
    void reset(void);
    virtual bool begin(uint8_t busyPin, uint8_t resetPin, uint8_t sdCSPin, uint8_t srCSPin, uint8_t dCSPin, uint8_t dcPin);
    virtual void powerOn(void);
    virtual void powerOff(void);

    void sendBW(uint8_t data[], uint16_t bytesToSend);
    void sendR(uint8_t data[], uint16_t bytesToSend);
    void refreshDisplay(bool wait = true);
    void updateDisplay(bool wait = true);

    //must be size of screen: i.e. data[xExt*yExt]
    void fillFromArray(eink_color_t data[], uint16_t arrLen, bool update = true, bool wait = true);
    void lineFromArray(uint16_t x, uint16_t y, uint16_t pixels, eink_color_t data[], bool update = true, bool wait = true);

    //must be size of screen
    void fillFromArray(uint8_t bwData[], uint8_t rData[], uint16_t arrLen, bool update = true, bool wait = true);
    void lineFromArray(uint16_t x, uint16_t y, uint16_t bytes, uint8_t bwData[], uint8_t rData[], bool update = true, bool wait = true);

    bool bmpFromSD(char * filename, byte whiteMin = 128, byte redMin = 128, bool update = true, bool wait = true);
    bool saveToSD(char * filename);
    bool loadFromSD(char * filename, bool update = true, bool wait = true);

    void pixel(uint16_t x0, uint16_t y0, eink_color_t color);
    void xline(uint16_t x0, uint16_t y0, uint16_t Len, eink_color_t color);
    void yline(uint16_t x, uint16_t y, uint16_t len, eink_color_t color);
    void rectangle(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, eink_color_t color, bool filled = false);
    void fillScreen(eink_color_t color);

    void delayWhileBusy(void);

  protected:
    uint8_t   _busyPin , _resetPin, _sdCSPin, _srCSPin, _dCSPin, _dcPin;
    uint16_t xExt;
    uint16_t yExt;
    uint16_t lineLength;
    uint16_t sizeBytes;
    uint16_t addressBW;
    uint16_t addressR;

    unsigned char lut_vcom0[15];
    unsigned char lut_w[15];
    unsigned char lut_b[15];
    unsigned char lut_g1[15];
    unsigned char lut_g2[15];
    unsigned char lut_vcom1[15];
    unsigned char lut_red0[15];
    unsigned char lut_red1[15];

    void sendCommand(uint8_t command);
    void sendData(uint8_t data);
    void SetLutBw(void);
    void SetLutRed(void);
    virtual void _sendBW(uint8_t data[], uint16_t bytesToSend);
    void _sendR(uint8_t data[], uint16_t bytesToSend);
    void _bmp24(uint8_t whiteMin, uint8_t redMin, uint16_t width, uint16_t height) ;

    void beginSRAM(void);
    void readSRAM (uint16_t address, uint8_t buff[], uint16_t bytesToRead);
    void writeSRAM (uint16_t address, uint8_t buff[], uint16_t bytesToSend);

};

#endif
