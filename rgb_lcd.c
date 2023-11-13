#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <stdint.h>

#include "rgb_lcd.h"

// Initialize an externally reserved struct
void lcd_init(struct Displaystate *disp)
{
    disp->_displayfunction = 0;
    disp->_displaycontrol = 0;
    disp->_displaymode = 0;
    disp->_initialized = 0;
    disp->_numlines = 0;
    disp->_currline = 0;
}

// Construct a displaystate struct reserving struct from heap
struct Displaystate *rgb_lcd(void)
{
    struct Displaystate *disp = malloc(sizeof(struct Displaystate));
    lcd_init(disp);
    return disp;
}

void lcd_begin(struct Displaystate *disp, uint32_t cols, uint32_t rows)
{
    disp->_displayfunction |= LCD_2LINE;

    disp->_numlines = 2;
    disp->_currline = 0;

    disp->_displaycontrol |= LCD_5x8DOTS;

    // SEE PAGE 45/46 FOR INITIALIZATION SPECIFICATION!
    // according to datasheet, we need at least 40ms after power rises above 2.7V
    // before sending commands. Sleep for 50ms just to be sure.
    sleep_ms(50);

    // this is according to the hitachi HD44780 datasheet
    // page 45 figure 23

    // Send function set command sequence
    _lcd_command(disp, LCD_FUNCTIONSET | disp->_displayfunction);
    sleep_us(4500);

    // second try
    _lcd_command(disp, LCD_FUNCTIONSET | disp->_displayfunction);
    sleep_us(150);

    // third try
    _lcd_command(disp, LCD_FUNCTIONSET | disp->_displayfunction);

    // finally, set # lines, font size, etc.
    _lcd_command(disp, LCD_FUNCTIONSET | disp->_displayfunction);

    // turnb the display on with no cursor or blinking default
    disp->_displaycontrol = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
    lcd_display(disp);

    // clear it off
    lcd_clear(disp);

    // Initialize to default text direction (for romance languages)
    disp->_displaymode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
    // set the entry mode
    _lcd_command(disp, LCD_ENTRYMODESET | disp->_displaymode);

    // check rgb chip model
    int32_t ret;
    uint8_t rxdata;
    ret = i2c_read_blocking(GROVE_I2C_INST, RGB_ADDRESS_V5, &rxdata, 1, false);
    if (ret > 0)
    {
        disp->rgb_chip_addr = RGB_ADDRESS_V5;
        _lcd_set_reg(disp, 0x00, 0x07); // reset the chip
        sleep_us(200);                  // wait 200 us to complete
        _lcd_set_reg(disp, 0x04, 0x15); // set all led always on
    }
    else
    {
        disp->rgb_chip_addr = RGB_ADDRESS;
        // backlight init
        _lcd_set_reg(disp, REG_MODE1, 0);
        // set LEDs controllable by both PWM and GRPPWM registers
        _lcd_set_reg(disp, REG_OUTPUT, 0xFF);
        // set MODE2 values
        // 0010 0000 -> 0x20  (DMBLNK to 1, ie blinky mode)
        _lcd_set_reg(disp, REG_MODE2, 0x20);
    }

    lcd_set_color_white(disp);
}

/********** high level commands, for the user! */
void lcd_clear(struct Displaystate *disp)
{
    _lcd_command(disp, LCD_CLEARDISPLAY); // clear display, set cursor position to zero
    sleep_us(2000);                       // this command takes a long time!
}

void lcd_home(struct Displaystate *disp)
{
    _lcd_command(disp, LCD_RETURNHOME); // set cursor position to zero
    sleep_us(2000);                     // this command takes a long time!
}

void lcd_set_cursor(struct Displaystate *disp, uint8_t col, uint8_t row)
{

    col = (row == 0 ? col | 0x80 : col | 0xc0);
    uint8_t dta[2] = {0x80, col};

    i2c_write_blocking(GROVE_I2C_INST, LCD_ADDRESS, &dta, 2, false);
}

// Turn the display on/off (quickly)
void lcd_no_display(struct Displaystate *disp)
{
    disp->_displaycontrol &= ~LCD_DISPLAYON;
    _lcd_command(disp, LCD_DISPLAYCONTROL | disp->_displaycontrol);
}

void lcd_display(struct Displaystate *disp)
{
    disp->_displaycontrol |= LCD_DISPLAYON;
    _lcd_command(disp, LCD_DISPLAYCONTROL | disp->_displaycontrol);
}

// Turns the underline cursor on/off
void lcd_no_cursor(struct Displaystate *disp)
{
    disp->_displaycontrol &= ~LCD_CURSORON;
    _lcd_command(disp, LCD_DISPLAYCONTROL | disp->_displaycontrol);
}

void lcd_cursor(struct Displaystate *disp)
{
    disp->_displaycontrol |= LCD_CURSORON;
    _lcd_command(disp, LCD_DISPLAYCONTROL | disp->_displaycontrol);
}

// Turn on and off the blinking cursor
void lcd_no_blink(struct Displaystate *disp)
{
    disp->_displaycontrol &= ~LCD_BLINKON;
    _lcd_command(disp, LCD_DISPLAYCONTROL | disp->_displaycontrol);
}
void lcd_blink(struct Displaystate *disp)
{
    disp->_displaycontrol |= LCD_BLINKON;
    _lcd_command(disp, LCD_DISPLAYCONTROL | disp->_displaycontrol);
}

// These commands scroll the display without changing the RAM
void lcd_scroll_display_left(struct Displaystate *disp)
{
    _lcd_command(disp, LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT);
}
void lcd_scroll_display_right(struct Displaystate *disp)
{
    _lcd_command(disp, LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT);
}

// This is for text that flows Left to Right
void lcd_left_to_right(struct Displaystate *disp)
{
    disp->_displaymode |= LCD_ENTRYLEFT;
    _lcd_command(disp, LCD_ENTRYMODESET | disp->_displaymode);
}

// This is for text that flows Right to Left
void lcd_right_to_left(struct Displaystate *disp)
{
    disp->_displaymode &= ~LCD_ENTRYLEFT;
    _lcd_command(disp, LCD_ENTRYMODESET | disp->_displaymode);
}

// This will 'right justify' text from the cursor
void lcd_autoscroll(struct Displaystate *disp)
{
    disp->_displaymode |= LCD_ENTRYSHIFTINCREMENT;
    _lcd_command(disp, LCD_ENTRYMODESET | disp->_displaymode);
}

// This will 'left justify' text from the cursor
void lcd_no_autoscroll(struct Displaystate *disp)
{
    disp->_displaymode &= ~LCD_ENTRYSHIFTINCREMENT;
    _lcd_command(disp, LCD_ENTRYMODESET | disp->_displaymode);
}

// Allows us to fill the first 8 CGRAM locations
// with custom characters
void lcd_create_char(struct Displaystate *disp, uint8_t location, uint8_t charmap[])
{

    location &= 0x7; // we only have 8 locations 0-7
    _lcd_command(disp, LCD_SETCGRAMADDR | (location << 3));

    uint8_t dta[9];
    dta[0] = 0x40;
    for (int i = 0; i < 8; i++)
    {
        dta[i + 1] = charmap[i];
    }
    i2c_write_blocking(GROVE_I2C_INST, LCD_ADDRESS, &dta, 9, false);
}

// Control the backlight LED blinking
void lcd_blink_led(struct Displaystate *disp)
{
    if (disp->rgb_chip_addr == RGB_ADDRESS_V5)
    {
        // attach all led to pwm1
        // blink period in seconds = (<reg 1> + 2) *0.128s
        // pwm1 on/off ratio = <reg 2> / 256
        _lcd_set_reg(disp, 0x04, 0x2a); // 0010 1010
        _lcd_set_reg(disp, 0x01, 0x06); // blink every second
        _lcd_set_reg(disp, 0x02, 0x7f); // half on, half off
    }
    else
    {
        // blink period in seconds = (<reg 7> + 1) / 24
        // on/off ratio = <reg 6> / 256
        _lcd_set_reg(disp, 0x07, 0x17); // blink every second
        _lcd_set_reg(disp, 0x06, 0x7f); // half on, half off
    }
}

void lcd_no_blink_led(struct Displaystate *disp)
{
    if (disp->rgb_chip_addr == RGB_ADDRESS_V5)
    {
        _lcd_set_reg(disp, 0x04, 0x15); // 0001 0101
    }
    else
    {
        _lcd_set_reg(disp, 0x07, 0x00);
        _lcd_set_reg(disp, 0x06, 0xff);
    }
}

/*********** mid level commands, for sending data/cmds */

// send command
void _lcd_command(struct Displaystate *disp, uint8_t value)
{
    uint8_t dta[2] = {0x80, value};
    i2c_write_blocking(GROVE_I2C_INST, LCD_ADDRESS, &dta, 2, false);
}

// send data
size_t lcd_write(struct Displaystate *disp, uint8_t value)
{
    uint8_t dta[2] = {0x40, value};
    i2c_write_blocking(GROVE_I2C_INST, LCD_ADDRESS, &dta, 2, false);
    return 1; // assume success
}

void _lcd_set_reg(struct Displaystate *disp, uint8_t reg, uint8_t dat)
{
    uint8_t dta[2] = {reg, dat};
    i2c_write_blocking(GROVE_I2C_INST, disp->rgb_chip_addr, &dta, 2, false);
}

void lcd_set_rgb(struct Displaystate *disp, uint8_t r, uint8_t g, uint8_t b)
{
    if (disp->rgb_chip_addr == RGB_ADDRESS_V5)
    {
        _lcd_set_reg(disp, 0x06, r);
        _lcd_set_reg(disp, 0x07, g);
        _lcd_set_reg(disp, 0x08, b);
    }
    else
    {
        _lcd_set_reg(disp, 0x04, r);
        _lcd_set_reg(disp, 0x03, g);
        _lcd_set_reg(disp, 0x02, b);
    }
}

void lcd_set_pwm(struct Displaystate *disp, uint8_t color, uint8_t pwm)
{
    switch (color)
    {
    case LCD_WHITE:
        lcd_set_rgb(disp, pwm, pwm, pwm);
        break;
    case LCD_RED:
        lcd_set_rgb(disp, pwm, 0, 0);
        break;
    case LCD_GREEN:
        lcd_set_rgb(disp, 0, pwm, 0);
        break;
    case LCD_BLUE:
        lcd_set_rgb(disp, 0, 0, pwm);
        break;
    default:
        break;
    }
}

const unsigned char color_define[4][3] = {
    {255, 255, 255}, // white
    {255, 0, 0},     // red
    {0, 255, 0},     // green
    {0, 0, 255},     // blue
};

void lcd_set_color(struct Displaystate *disp, unsigned char color)
{
    if (color > 3)
    {
        return;
    }
    lcd_set_rgb(disp, color_define[color][0], color_define[color][1], color_define[color][2]);
}

void lcd_set_color_all(struct Displaystate *disp)
{
    lcd_set_rgb(disp, 0, 0, 0);
}

void lcd_set_color_white(struct Displaystate *disp)
{
    lcd_set_rgb(disp, 255, 255, 255);
}
