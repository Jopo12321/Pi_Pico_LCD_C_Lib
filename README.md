# LCD HD44780 C library for Raspberry Pi Pico

This is a C language library for controlling HD44780-based LCDs using the Raspberry Pi Pico (RP2040, RP2350). It provides an easy interface to connect and control character LCDs (16x2, 20x4, etc.) with the Raspberry Pi Pico using standard 4-bit or 8-bit modes. This library is inspired by the Arduino [LiquidCrystal](https://www.arduino.cc/reference/en/libraries/liquidcrystal/) library, and implements some of it's functions.

# Features
- Simple and easy-to-use API for interfacing with HD44780 LCD displays.
- Supports both 4-bit and 8-bit communication modes with optional use of R/W pin.
- Provides functions to display strings, control cursor position, and manage the display.
- Compatible with 16x2 and 20x4 LCD configurations.
- Written for use with Raspberry Pi Pico's SDK

# Installation

- Add the `LCD_HD44780U.h` and `LCD_HD44780U.c` files to your source folder.
- Include the header in your main program file:
    ```c
    #include "LCD_HD44780U.h"
    ```

# Usage
## Initialization

The library supports both **4-bit** and **8-bit** modes for communication with the LCD. Depending on the number of available GPIO pins, you can choose either of the following initialization methods.

- **4-bit mode**: This mode uses fewer GPIO pins (ideal for saving pins in your setup).
- **8-bit mode**: This mode uses all 8 data lines but provides slightly faster data transfer.

You can also opt not to use the **R/W** pin by passing `255` as its value in the code, but ensure it is **physically connected to ground**.

### 4-bit Initialization Example

In 4-bit mode, only four data pins are used. The `lcd_init_4bit()` function initializes the LCD as follows:

```c
#include "LCD_HD44780U.h"

// Define the dimensions of the LCD
#define LCD_COLS 16  // Number of columns on the LCD
#define LCD_ROWS 2   // Number of rows on the LCD

// Define the GPIO pins for LCD control
#define LCD_RS 6     // Register select pin
#define LCD_RW 5     // Read/write pin (pass 255 if you are not using it)
#define LCD_EN 4     // Enable pin

// Define the GPIO pins for LCD data (D4-D7)
#define LCD_D4 0     // Data pin D4
#define LCD_D5 1     // Data pin D5
#define LCD_D6 2     // Data pin D6
#define LCD_D7 3     // Data pin D7

int main() {
    // 4-bit initialization
    LCD_Handle *lcd = lcd_init_4bit(LCD_COLS, LCD_ROWS, LCD_5x8DOTS, LCD_RS, LCD_RW, LCD_EN, LCD_D4, LCD_D5, LCD_D6, LCD_D7);
        
    // Print a message
    lcd_write_string(lcd, "Hello World!");

    while (1) {
        // Main loop
    }

    return 0;
}
```

### 8-bit Initialization Example

In 8-bit mode, all eight data lines (D0 to D7) are used. This provides faster communication, but requires more GPIO pins:

```c
#include "LCD_HD44780U.h"

// Define the dimensions of the LCD
#define LCD_COLS 16  // Number of columns on the LCD
#define LCD_ROWS 2   // Number of rows on the LCD

// Define the GPIO pins for LCD control
#define LCD_RS 10    // Register select pin
#define LCD_RW 9     // Read/write pin (pass 255 if you are not using it)
#define LCD_EN 8     // Enable pin

// Define the GPIO pins for LCD data (D0-D7)
#define LCD_D0 0     // Data pin D0
#define LCD_D1 1     // Data pin D1
#define LCD_D2 2     // Data pin D2
#define LCD_D3 3     // Data pin D3
#define LCD_D4 4     // Data pin D4
#define LCD_D5 5     // Data pin D5
#define LCD_D6 6     // Data pin D6
#define LCD_D7 7     // Data pin D7

int main() {
    // 4-bit initialization
    LCD_Handle *lcd = lcd_init_8bit(LCD_COLS, LCD_ROWS, LCD_5x8DOTS, LCD_RS, LCD_RW, LCD_EN, LCD_D0, LCD_D1, LCD_D2, LCD_D3, LCD_D4, LCD_D5, LCD_D6, LCD_D7);
        
    // Print a message
    lcd_write_string(lcd, "Hello World!");

    while (1) {
        // Main loop
    }

    return 0;
}
```
## Functions

The following are the main functions provided by the library. These functions allow you to initialize the LCD, control its display settings, and write text or custom characters.

### Initialization Functions

#### `LCD_Handle *lcd_init_8bit(uint8_t cols, uint8_t rows, uint8_t charsize, uint8_t rs, uint8_t rw, uint8_t enable, uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3, uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7)`

Initializes the LCD in 8-bit mode.

- **Parameters:**
  - `cols`: Number of columns on the display.
  - `rows`: Number of rows on the display.
  - `charsize`: Character size (5x8 dots or 5x10 dots).
  - `rs`: GPIO pin for Register Select.
  - `rw`: GPIO pin for Read/Write (pass `255` if not used and ground the R/W pin).
  - `enable`: GPIO pin for Enable.
  - `d0`–`d7`: GPIO pins for data lines.
- **Returns:** Pointer to the initialized `LCD_Handle`, or `NULL` if initialization failed.

#### `LCD_Handle *lcd_init_4bit(uint8_t cols, uint8_t rows, uint8_t charsize, uint8_t rs, uint8_t rw, uint8_t enable, uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7)`

Initializes the LCD in 4-bit mode.

- **Parameters:**
  - `cols`: Number of columns on the display.
  - `rows`: Number of rows on the display.
  - `charsize`: Character size (5x8 dots or 5x10 dots).
  - `rs`: GPIO pin for Register Select.
  - `rw`: GPIO pin for Read/Write (pass `255` if not used and ground the R/W pin).
  - `enable`: GPIO pin for Enable.
  - `d4`–`d7`: GPIO pins for data lines in 4-bit mode.
- **Returns:** Pointer to the initialized `LCD_Handle`, or `NULL` if initialization failed.

#### `LCD_Handle *lcd_deinit(LCD_Handle *handle)`

Deinitializes the LCD and frees resources.

- **Parameters:** 
  - `handle`: Pointer to the `LCD_Handle` structure.
- **Returns:** Always returns `NULL` after deinitializing the LCD.

### Display Control Functions

#### `void lcd_clear(LCD_Handle *handle)`

Clears the display and moves the cursor to the home position.

#### `void lcd_home(LCD_Handle *handle)`

Moves the cursor to the home position (top-left corner).

#### `void lcd_display_on(LCD_Handle *handle)`

Turns on the display.

#### `void lcd_display_off(LCD_Handle *handle)`

Turns off the display.

#### `void lcd_blink_on(LCD_Handle *handle)`

Enables cursor blinking.

#### `void lcd_blink_off(LCD_Handle *handle)`

Disables cursor blinking.

#### `void lcd_cursor_on(LCD_Handle *handle)`

Shows the cursor on the display.

#### `void lcd_cursor_off(LCD_Handle *handle)`

Hides the cursor.

### Cursor and Text Control Functions

#### `void lcd_set_cursor(LCD_Handle *handle, uint8_t col, uint8_t row)`

Moves the cursor to a specific position.

- **Parameters:**
  - `col`: The column position (0-based).
  - `row`: The row position (0-based).

#### `void lcd_write_char(LCD_Handle *handle, char symbol)`

Displays a single character at the current cursor position.

#### `void lcd_write_string(LCD_Handle *handle, char *text)`

Writes a null-terminated string starting at the current cursor position.

- **Parameters:**
  - `text`: The string to display.

#### `void lcd_write_char_at(LCD_Handle *handle, char symbol, uint8_t col, uint8_t row)`

Writes a single character at a specified position on the display.

- **Parameters:**
  - `col`: The column position (0-based).
  - `row`: The row position (0-based).
  - `symbol`: The character to display.

#### `void lcd_write_string_at(LCD_Handle *handle, char *text, uint8_t col, uint8_t row)`

Writes a string at a specified position on the display.

- **Parameters:**
  - `text`: The string to display.
  - `col`: The column position (0-based).
  - `row`: The row position (0-based).

### Scrolling Functions

#### `void lcd_scroll_display_left(LCD_Handle *handle)`

Scrolls the display content one position to the left.

#### `void lcd_scroll_display_right(LCD_Handle *handle)`

Scrolls the display content one position to the right.

### Text Direction and Auto-Scrolling Functions

#### `void lcd_left_to_right(LCD_Handle *handle)`

Sets text direction to left-to-right.

#### `void lcd_right_to_left(LCD_Handle *handle)`

Sets text direction to right-to-left.

#### `void lcd_autoscroll_on(LCD_Handle *handle)`

Enables auto-scrolling, so text will automatically scroll to the next line.

#### `void lcd_autoscroll_off(LCD_Handle *handle)`

Disables auto-scrolling.

### Custom Characters

#### `void lcd_create_char(LCD_Handle *handle, uint8_t num, uint8_t *data)`

Creates a custom character pattern in the LCD’s CGRAM.

- **Parameters:**
  - `num`: Character code (0–7).
  - `data`: Array of 8 bytes representing the character pattern (8 rows).

## Example

An example program demonstrating the use of all the functions provided by this library is available in the file [`Example.c`](./Example.c). 
You can also watch a video demonstration of the library in action on [**YouTube**](https://youtu.be/dUbcyziSP4U).

# Credits
This library was inspired by the Arduino [LiquidCrystal](https://www.arduino.cc/reference/en/libraries/liquidcrystal/) written by **Arduino Team**. 

Special thanks to the creators of the Raspberry Pi Pico microcontroller and the broader Raspberry Pi and Arduino communities for their resources and support.

# License
This library is licensed under the MIT License. See the [LICENSE](./LICENSE) file for more details.