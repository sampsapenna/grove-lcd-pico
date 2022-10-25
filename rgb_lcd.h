#ifndef __RGB_LCD_H__
#define __RGB_LCD_H__

#include <inttypes.h>

// i2c to use in Raspberry Pi Pico
#ifdef GROVE_PICO_SHIELD
#define GROVE_I2C_INST &i2c1_inst
#else
#define GROVE_I2C_INST &i2c0_inst
#endif

// Device I2C Arress
#define LCD_ADDRESS (0x7c >> 1)
#define RGB_ADDRESS (0xc4 >> 1)
#define RGB_ADDRESS_V5 (0x30)

// color define
#define WHITE 0
#define RED 1
#define GREEN 2
#define BLUE 3

#define REG_MODE1 0x00
#define REG_MODE2 0x01
#define REG_OUTPUT 0x08

// commands
#define LCD_CLEARDISPLAY 0x01
#define LCD_RETURNHOME 0x02
#define LCD_ENTRYMODESET 0x04
#define LCD_DISPLAYCONTROL 0x08
#define LCD_CURSORSHIFT 0x10
#define LCD_FUNCTIONSET 0x20
#define LCD_SETCGRAMADDR 0x40
#define LCD_SETDDRAMADDR 0x80

// flags for display entry mode
#define LCD_ENTRYRIGHT 0x00
#define LCD_ENTRYLEFT 0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00

// flags for display on/off control
#define LCD_DISPLAYON 0x04
#define LCD_DISPLAYOFF 0x00
#define LCD_CURSORON 0x02
#define LCD_CURSOROFF 0x00
#define LCD_BLINKON 0x01
#define LCD_BLINKOFF 0x00

// flags for display/cursor shift
#define LCD_DISPLAYMOVE 0x08
#define LCD_CURSORMOVE 0x00
#define LCD_MOVERIGHT 0x04
#define LCD_MOVELEFT 0x00

// flags for function set
#define LCD_8BITMODE 0x10
#define LCD_4BITMODE 0x00
#define LCD_2LINE 0x08
#define LCD_1LINE 0x00
#define LCD_5x10DOTS 0x04
#define LCD_5x8DOTS 0x00

struct Displaystate
{
    uint32_t rgb_chip_addr;
    uint32_t _displayfunction;
    uint32_t _displaycontrol;
    uint32_t _displaymode;
    uint32_t _initialized;
    uint32_t _numlines;
    uint32_t _currline;
};

// Constructor / initiator
struct Displaystate *rgb_lcd(void);
void rgb_init(struct Displaystate *disp);

void begin(struct Displaystate *disp, uint32_t cols, uint32_t rows);

void clear(struct Displaystate *disp);
void home(struct Displaystate *disp);

void noDisplay(struct Displaystate *disp);
void display(struct Displaystate *disp);
void noBlink(struct Displaystate *disp);
void blink(struct Displaystate *disp);
void noCursor(struct Displaystate *disp);
void cursor(struct Displaystate *disp);
void scrollDisplayLeft(struct Displaystate *disp);
void scrollDisplayRight(struct Displaystate *disp);
void leftToRight(struct Displaystate *disp);
void rightToLeft(struct Displaystate *disp);
void autoscroll(struct Displaystate *disp);
void noAutoscroll(struct Displaystate *disp);

void createChar(struct Displaystate *disp, uint8_t, uint8_t[]);
void setCursor(struct Displaystate *disp, uint8_t, uint8_t);

size_t write(struct Displaystate *disp, uint8_t);
void command(struct Displaystate *disp, uint8_t);

void setRGB(struct Displaystate *disp, unsigned char r, unsigned char g, unsigned char b); // set rgb
void setPWM(struct Displaystate *disp, unsigned char color, unsigned char pwm);            // set pwm

void setColor(struct Displaystate *disp, unsigned char color);
void setColorAll(struct Displaystate *disp);
void setColorWhite(struct Displaystate *disp);

// blink the LED backlight
void blinkLED(struct Displaystate *disp);
void noBlinkLED(struct Displaystate *disp);

#endif