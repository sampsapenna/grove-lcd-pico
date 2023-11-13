# Grove LCD backlight module library – Raspberry Pico SDK port
This is a library for driving the Grove LCD backlight, ported from the Arduino
library provided by Seeed Studio. The library works by same logic, but is
re-written to use Pico SDK's i2c library instead of `Wire.h`, and using C
instead of C++.

The original library can be found [in Seeed Studio github](https://github.com/Seeed-Studio/Grove_LCD_RGB_Backlight).
All additions are provided under the original license.

An example pico program is provided in [grove-lcd-pico-example repository.](https://github.com/sampsapenna/grove-lcd-pico-example)

## Usage
The port can be easily integrated into a Pico SDK project as a git submodule.
```bash
git submodule add https://github.com/sampsapenna/grove-lcd-pico.git
git submodule update
```
After cloning the repository you need to add the library to `CMakeLists.txt`
used when building the project. Add the following lines before `target_link_libraries`:
```cmake
add_subdirectory("grove-lcd-pico")

target_include_directories(rgb_lcd PUBLIC "grove-lcd-pico")
target_link_directories(rgb_lcd PUBLIC "grove-lcd-pico")
```

Additionally you will need to add the required libraries to linking, by
adding `hardware_i2c rgb_lcd pico_binary_info` to `target_link_libraries`.

Libary does not initialize i2c, but assumes that program initializes i2c
for it. Default initialization, sourced from Raspberry Pi Pico documentation,
can look e.g. like this:
```c
    i2c_init(i2c_default, 100 * 1000);
    gpio_set_function(PICO_DEFAULT_I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(PICO_DEFAULT_I2C_SDA_PIN);
    gpio_pull_up(PICO_DEFAULT_I2C_SCL_PIN);
    // Make the I2C pins available to picotool
    bi_decl(bi_2pins_with_func(PICO_DEFAULT_I2C_SDA_PIN, PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C));
```
Header file to include is `rgb_lcd.h`, same as in the Arduino library.

Library defaults to using `i2c0_inst`, which is fine for most purposes. Some
shields only expose `i2c0_inst`, so this parameter is made configurable via
`GROVE_PICO_SHIELD` flag on compile-time.

Some functions, especially the high level ones, use `sleep_ms` function for timing.
As such, these funcions should only be used outside time-critical code, e.g. in
a `setup()` function, since they can mess up the timing of other unrelated code
in the program. For implementation details, inspect `rgb_lcd.c` for which funcions
use `sleep_ms` for timing.

## Bugs
The library has been tested with the grove blue/white display, but not the
RGB library. This means there might be bugs. Additionally, the drawing logic
could be improved, `lcd_write()` function currently drawing a single character
at a time.

If you encounter a bug, go ahead and open an issue.
