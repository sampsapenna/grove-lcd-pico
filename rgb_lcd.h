#ifndef __RGB_LCD_H__
#define __RGB_LCD_H__

#include <inttypes.h>

// i2c to use in Raspberry Pi Pico
#ifdef GROVE_PICO_SHIELD
#define GROVE_I2C_INST &i2c1_inst
#else
#define GROVE_I2C_INST &i2c0_inst
#endif

// Device I2C Address
#define LCD_ADDRESS (0x7c >> 1)
#define RGB_ADDRESS (0xc4 >> 1)
#define RGB_ADDRESS_V5 (0x30)

// color define
#define LCD_WHITE 0
#define LCD_RED 1
#define LCD_GREEN 2
#define LCD_BLUE 3

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

/*
Display state structure. Underscore denotes internal values.
*/
typedef struct Displaystate
{
    uint32_t rgb_chip_addr;
    uint32_t _displayfunction;
    uint32_t _displaycontrol;
    uint32_t _displaymode;
    uint32_t _initialized;
    uint32_t _numlines;
    uint32_t _currline;
} Displaystate;

/*******************************************/
/* Initialization and lifecycle functions. */
/*******************************************/
// Initialize an externally reserved display struct.
struct Displaystate *rgb_lcd(void);
// Create an initialize the display state.
void rgb_lcd_init(struct Displaystate *disp);

/**********************/
/* Control functions. */
/**********************/
// Bring the display online gracefully.
void lcd_begin(struct Displaystate *disp, uint32_t cols, uint32_t rows);
// Clear display contents.
void lcd_clear(struct Displaystate *disp);
// Return display cursor to home.
void lcd_home(struct Displaystate *disp);
// Move the cursor to a specific position.
void lcd_set_cursor(struct Displaystate *disp, uint8_t, uint8_t);

/****************************/
/* Configuration functions. */
/****************************/
// Turn off the display backlight.
void lcd_no_display(struct Displaystate *disp);
// Turn on the display backlight.
void lcd_display(struct Displaystate *disp);
// Turn off blinking cursor.
void lcd_no_blink(struct Displaystate *disp);
// Turn on blinking cursor.
void lcd_blink(struct Displaystate *disp);
// Turn off cursor.
void lcd_no_cursor(struct Displaystate *disp);
// Turn on the cursor.
void lcd_cursor(struct Displaystate *disp);
// Scroll the display to the left.
void lcd_scroll_display_left(struct Displaystate *disp);
// Scroll the display to the right.
void lcd_scroll_display_right(struct Displaystate *disp);
// Toggle writing from left to right.
void lcd_left_to_right(struct Displaystate *disp);
// Toggle writing from right to left.
void lcd_righ_to_left(struct Displaystate *disp);
// Toggle automatic scrolling on.
void lcd_autoscroll(struct Displaystate *disp);
// Toggle automatic scrolling off.
void lcd_no_autoscroll(struct Displaystate *disp);
// Create a custom mapped character for the display.
void lcd_create_char(struct Displaystate *disp, uint8_t, uint8_t[]);
// Blink the LED backlight.
void lcd_blink_led(struct Displaystate *disp);
// Don't blink the LED backlight.
void lcd_no_blink_led(struct Displaystate *disp);

/*******************************/
/* RGB specific configuration. */
/*******************************/
// set RGB color to a specific value.
void lcd_set_rgb(struct Displaystate *disp, unsigned char r, unsigned char g, unsigned char b); // set rgb
// set RGB brightness to a specific value
void lcd_set_pwm(struct Displaystate *disp, unsigned char color, unsigned char pwm); // set pwm
// Set color to a pre-defined value.
void lcd_set_color(struct Displaystate *disp, unsigned char color);
// Zero all the color values.
void lcd_set_color_all(struct Displaystate *disp);
// Set all colors to the maximum value.
void lcd_set_color_white(struct Displaystate *disp);

/****************************/
/* Content write functions. */
/****************************/
// Write a character to the screen.
size_t lcd_write(struct Displaystate *disp, uint8_t);

/***********************/
/* Internal functions. */
/***********************/
// Write a command to the screen.
void _lcd_command(struct Displaystate *disp, uint8_t);
// Write a value to a display register.
void _lcd_set_reg(struct Displaystate *disp, uint8_t reg, uint8_t dat);

#endif
