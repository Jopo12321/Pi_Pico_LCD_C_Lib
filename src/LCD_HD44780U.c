/*
 * SPDX-FileCopyrightText: 2024 Jozef Kromka <jozef.kromka22@gmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

// ########################################################################## //
//                                                                            //
//             Raspberry Pi Pico LCD HD44780U library source file             //
//                                                                            //
// ########################################################################## //

#include "LCD_HD44780U.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pico/stdlib.h"

// ########################################################################## //
//                                                                            //
//        Private functions definition (not listed in LCD_HD44780U.h)         //
//                                                                            //
// ########################################################################## //

LCD_Handle *_lcd_init(uint8_t cols, uint8_t rows, uint8_t charsize, uint8_t rs,
                      uint8_t rw, uint8_t enable, uint8_t d0, uint8_t d1,
                      uint8_t d2, uint8_t d3, uint8_t d4, uint8_t d5,
                      uint8_t d6, uint8_t d7, bool eightbitmode);
void _lcd_setup(LCD_Handle *handle, uint8_t cols, uint8_t rows,
                uint8_t charsize);
void _lcd_init_pins(LCD_Handle *handle);
void _lcd_deinit_pins(LCD_Handle *handle);

void _lcd_send_command(LCD_Handle *handle, uint8_t command);
void _lcd_send_data(LCD_Handle *handle, uint8_t data);
uint8_t _lcd_read_command(LCD_Handle *handle);
uint8_t _lcd_read_data(LCD_Handle *handle);

void _lcd_write_8_bits(LCD_Handle *handle, uint8_t data);
void _lcd_write_4_bits(LCD_Handle *handle, uint8_t data);
uint8_t _lcd_read_8_bits(LCD_Handle *handle);
uint8_t _lcd_read_4_bits(LCD_Handle *handle);

bool _lcd_busy(LCD_Handle *handle);

// ########################################################################## //
//                                                                            //
//                       Public function implementation                       //
//                                                                            //
// ########################################################################## //

/**
 * @brief Initializes the LCD in 8-bit mode.
 *
 * This function sets up the LCD display for 8-bit operation, including configuring
 * the pins and setting up the display with the specified parameters.
 *
 * @param cols Number of columns of the LCD display.
 * @param rows Number of rows of the LCD display.
 * @param charsize Character size (5x8 dots [LCD_5x8DOTS] or 5x10 dots [LCD_5x10DOTS]).
 * @param rs GPIO pin number for the RS (Register Select) pin.
 * @param rw GPIO pin number for the RW (Read/Write) pin (255 if not used).
 * @param enable GPIO pin number for the Enable pin.
 * @param d0 GPIO pin number for data line 0.
 * @param d1 GPIO pin number for data line 1.
 * @param d2 GPIO pin number for data line 2.
 * @param d3 GPIO pin number for data line 3.
 * @param d4 GPIO pin number for data line 4.
 * @param d5 GPIO pin number for data line 5.
 * @param d6 GPIO pin number for data line 6.
 * @param d7 GPIO pin number for data line 7.
 * @return LCD_Handle* Pointer to the initialized LCD handle, or NULL if initialization failed.
 */
LCD_Handle *lcd_init_8bit(uint8_t cols, uint8_t rows, uint8_t charsize,
                          uint8_t rs, uint8_t rw, uint8_t enable, uint8_t d0,
                          uint8_t d1, uint8_t d2, uint8_t d3, uint8_t d4,
                          uint8_t d5, uint8_t d6, uint8_t d7) {
  return _lcd_init(cols, rows, charsize, rs, rw, enable, d0, d1, d2, d3, d4, d5,
                   d6, d7, true);
}

/**
 * @brief Initializes the LCD in 4-bit mode.
 *
 * This function sets up the LCD display for 4-bit operation, including configuring
 * the pins and setting up the display with the specified parameters.
 *
 * @param cols Number of columns of the LCD display.
 * @param rows Number of rows of the LCD display.
 * @param charsize Character size (5x8 dots or 5x10 dots).
 * @param rs GPIO pin number for the RS (Register Select) pin.
 * @param rw GPIO pin number for the RW (Read/Write) pin (255 if not used).
 * @param enable GPIO pin number for the Enable pin.
 * @param d4 GPIO pin number for data line 4.
 * @param d5 GPIO pin number for data line 5.
 * @param d6 GPIO pin number for data line 6.
 * @param d7 GPIO pin number for data line 7.
 * @return LCD_Handle* Pointer to the initialized LCD handle, or NULL if initialization failed.
 */
LCD_Handle *lcd_init_4bit(uint8_t cols, uint8_t rows, uint8_t charsize,
                          uint8_t rs, uint8_t rw, uint8_t enable, uint8_t d4,
                          uint8_t d5, uint8_t d6, uint8_t d7) {
  return _lcd_init(cols, rows, charsize, rs, rw, enable, d4, d5, d6, d7, 0, 0,
                   0, 0, false);
}

/**
 * @brief Deinitializes the LCD and frees the associated resources.
 *
 * This function clears the display, returns the cursor to home position, turns off the display,
 * deinitializes the GPIO pins, and frees the memory allocated for the LCD handle.
 *
 * @param handle Pointer to the LCD handle to be deinitialized.
 * @return LCD_Handle* NULL (always returns NULL as the handle is freed).
 */
LCD_Handle *lcd_deinit(LCD_Handle *handle) {
  if (handle != NULL) {
    lcd_clear(handle);
    lcd_home(handle);
    lcd_display_off(handle);
    _lcd_deinit_pins(handle);
    free(handle);
  }
  return NULL;
}

/**
 * @brief Clears the LCD display.
 *
 * This function sends a command to clear the display and waits for a short period to ensure
 * that the command is processed.
 *
 * @param handle Pointer to the LCD handle.
 */
void lcd_clear(LCD_Handle *handle) {
  if (handle == NULL) {
    return;
  }
  _lcd_send_command(handle, LCD_CLEARDISPLAY);
  sleep_ms(5);  // Wait for the display to clear.
}

/**
 * @brief Sets the cursor to the home position.
 *
 * This function sends a command to move the cursor to the home position (top-left corner)
 * and waits for a short period to ensure that the command is processed.
 *
 * @param handle Pointer to the LCD handle.
 */
void lcd_home(LCD_Handle *handle) {
  if (handle == NULL) {
    return;
  }
  _lcd_send_command(handle, LCD_RETURNHOME);
  sleep_ms(5);  // Wait for the cursor to return home.
}

/**
 * @brief Turns off the LCD display.
 *
 * This function sends a command to turn off the display.
 *
 * @param handle Pointer to the LCD handle.
 */
void lcd_display_off(LCD_Handle *handle) {
  if (handle == NULL) {
    return;
  }
  handle->_displaycontrol &= ~LCD_DISPLAYON;
  _lcd_send_command(handle, LCD_DISPLAYCONTROL | handle->_displaycontrol);
}

/**
 * @brief Turns on the LCD display.
 *
 * This function sends a command to turn on the display.
 *
 * @param handle Pointer to the LCD handle.
 */
void lcd_display_on(LCD_Handle *handle) {
  if (handle == NULL) {
    return;
  }
  handle->_displaycontrol |= LCD_DISPLAYON;
  _lcd_send_command(handle, LCD_DISPLAYCONTROL | handle->_displaycontrol);
}

/**
 * @brief Turns off the cursor blinking.
 *
 * This function sends a command to disable the cursor blinking.
 *
 * @param handle Pointer to the LCD handle.
 */
void lcd_blink_off(LCD_Handle *handle) {
  if (handle == NULL) {
    return;
  }
  handle->_displaycontrol &= ~LCD_BLINKON;
  _lcd_send_command(handle, LCD_DISPLAYCONTROL | handle->_displaycontrol);
}

/**
 * @brief Turns on the cursor blinking.
 *
 * This function sends a command to enable the cursor blinking.
 *
 * @param handle Pointer to the LCD handle.
 */
void lcd_blink_on(LCD_Handle *handle) {
  if (handle == NULL) {
    return;
  }
  handle->_displaycontrol |= LCD_BLINKON;
  _lcd_send_command(handle, LCD_DISPLAYCONTROL | handle->_displaycontrol);
}

/**
 * @brief Turns off the cursor.
 *
 * This function sends a command to hide the cursor.
 *
 * @param handle Pointer to the LCD handle.
 */
void lcd_cursor_off(LCD_Handle *handle) {
  if (handle == NULL) {
    return;
  }
  handle->_displaycontrol &= ~LCD_CURSORON;
  _lcd_send_command(handle, LCD_DISPLAYCONTROL | handle->_displaycontrol);
}

/**
 * @brief Turns on the cursor.
 *
 * This function sends a command to show the cursor.
 *
 * @param handle Pointer to the LCD handle.
 */
void lcd_cursor_on(LCD_Handle *handle) {
  if (handle == NULL) {
    return;
  }
  handle->_displaycontrol |= LCD_CURSORON;
  _lcd_send_command(handle, LCD_DISPLAYCONTROL | handle->_displaycontrol);
}

/**
 * @brief Scrolls the display content to the left.
 *
 * This function sends a command to scroll the entire display content one position to the left.
 *
 * @param handle Pointer to the LCD handle.
 */
void lcd_scroll_display_left(LCD_Handle *handle) {
  if (handle == NULL) {
    return;
  }
  _lcd_send_command(handle, LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT);
}

/**
 * @brief Scrolls the display content to the right.
 *
 * This function sends a command to scroll the entire display content one position to the right.
 *
 * @param handle Pointer to the LCD handle.
 */
void lcd_scroll_display_right(LCD_Handle *handle) {
  if (handle == NULL) {
    return;
  }
  _lcd_send_command(handle, LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT);
}

/**
 * @brief Sets the text direction to left-to-right.
 *
 * This function configures the display to write text from left to right.
 *
 * @param handle Pointer to the LCD handle.
 */
void lcd_left_to_right(LCD_Handle *handle) {
  if (handle == NULL) {
    return;
  }
  handle->_displaymode |= LCD_ENTRYLEFT;
  _lcd_send_command(handle, LCD_ENTRYMODESET | handle->_displaymode);
}

/**
 * @brief Sets the text direction to right-to-left.
 *
 * This function configures the display to write text from right to left.
 *
 * @param handle Pointer to the LCD handle.
 */
void lcd_right_to_left(LCD_Handle *handle) {
  if (handle == NULL) {
    return;
  }
  handle->_displaymode &= ~LCD_ENTRYLEFT;
  _lcd_send_command(handle, LCD_ENTRYMODESET | handle->_displaymode);
}

/**
 * @brief Disables auto-scrolling.
 *
 * This function sends a command to disable auto-scrolling, meaning the text will not automatically scroll.
 *
 * @param handle Pointer to the LCD handle.
 */
void lcd_autoscroll_off(LCD_Handle *handle) {
  if (handle == NULL) {
    return;
  }
  handle->_displaymode &= ~LCD_ENTRYSHIFTINCREMENT;
  _lcd_send_command(handle, LCD_ENTRYMODESET | handle->_displaymode);
}

/**
 * @brief Enables auto-scrolling.
 *
 * This function sends a command to enable auto-scrolling, meaning the text will automatically scroll.
 *
 * @param handle Pointer to the LCD handle.
 */
void lcd_autoscroll_on(LCD_Handle *handle) {
  if (handle == NULL) {
    return;
  }
  handle->_displaymode |= LCD_ENTRYSHIFTINCREMENT;
  _lcd_send_command(handle, LCD_ENTRYMODESET | handle->_displaymode);
}

/**
 * @brief Sets the cursor to a specific position.
 *
 * This function sends a command to move the cursor to a specified column and row.
 *
 * @param handle Pointer to the LCD handle.
 * @param col Column position (0-based index).
 * @param row Row position (0-based index).
 */
void lcd_set_cursor(LCD_Handle *handle, uint8_t col, uint8_t row) {
  if (handle == NULL) {
    return;
  }
  const size_t max_lines =
      sizeof(handle->_row_offsets) / sizeof(*handle->_row_offsets);
  if (row >= max_lines) {
    row = max_lines - 1;
  }
  if (row >= handle->_numlines) {
    row = handle->_numlines - 1;
  }
  _lcd_send_command(handle,
                    LCD_SETDDRAMADDR | (col + handle->_row_offsets[row]));
}

/**
 * @brief Displays a single character on the LCD.
 *
 * This function sends a character to the LCD for display at the current cursor position.
 *
 * @param handle Pointer to the LCD handle.
 * @param symbol Character to be displayed.
 */
void lcd_write_char(LCD_Handle *handle, char symbol) {
  if (handle == NULL) {
    return;
  }
  _lcd_send_data(handle, symbol);
}

/**
 * @brief Writes a string to the LCD.
 *
 * This function sends a string of characters to the LCD for display starting from the current cursor position.
 *
 * @param handle Pointer to the LCD handle.
 * @param text Null-terminated string to be displayed.
 */
void lcd_write_string(LCD_Handle *handle, char *text) {
  if (handle == NULL) {
    return;
  }
  for (size_t i = 0; i < strlen(text); i++) {
    lcd_write_char(handle, text[i]);
  }
}

/**
 * @brief Writes a single character to a specific position on the LCD.
 *
 * This function sets the cursor to the specified column and row on the LCD,
 * and writes the given character at that position.
 *
 * @param handle Pointer to the LCD handle.
 * @param symbol Character to be displayed.
 * @param col Column position (0-based index).
 * @param row Row position (0-based index).
 */
void lcd_write_char_at(LCD_Handle *handle, char symbol, uint8_t col,
                       uint8_t row) {
  if (handle == NULL) {
    return;
  }
  lcd_set_cursor(handle, col, row);
  lcd_write_char(handle, symbol);
}

/**
 * @brief Writes a string to a specific position on the LCD.
 *
 * This function sets the cursor to the specified column and row on the LCD,
 * and writes the given string starting from that position.
 *
 * @param handle Pointer to the LCD handle.
 * @param text Null-terminated string to be displayed.
 * @param col Column position (0-based index).
 * @param row Row position (0-based index).
 */
void lcd_write_string_at(LCD_Handle *handle, char *text, uint8_t col,
                         uint8_t row) {
  if (handle == NULL) {
    return;
  }
  lcd_set_cursor(handle, col, row);
  lcd_write_string(handle, text);
}

/**
 * @brief Creates a custom character on the LCD.
 *
 * This function defines a new custom character pattern in the LCD's CGRAM (Character Generator RAM).
 * The pattern is specified by the `data` array and can be used later to display the custom character.
 *
 * @param handle Pointer to the LCD handle.
 * @param num Character code for the custom character (0-7 for most LCDs).
 * @param data Array of 8 bytes representing the custom character pattern. Each byte corresponds
 *             to one row of the character (8 rows in total).
 */
void lcd_create_char(LCD_Handle *handle, uint8_t num, uint8_t *data) {
  if (handle == NULL) {
    return;
  }
  uint8_t ddram_address = 0;
  if (handle->_rw_pin != 255) {
    ddram_address = _lcd_read_command(handle) & 0x7F;
  }
  uint8_t gcram_address = (num & 0x7) << 3;
  for (size_t i = 0; i < 8; i++) {
    _lcd_send_command(handle, LCD_SETCGRAMADDR | (gcram_address + i));
    _lcd_send_data(handle, data[i]);
  }
  if (handle->_rw_pin != 255) {
    _lcd_send_command(handle, LCD_SETDDRAMADDR | ddram_address);
  }
}

// ########################################################################## //
//                                                                            //
//                      Private function implementation                       //
//                                                                            //
// ########################################################################## //

/**
 * @brief Initializes the LCD with the specified settings and returns a handle.
 *
 * This function sets up the LCD controller with the given parameters such as
 * the number of columns and rows, character size, and GPIO pin assignments.
 * It performs the necessary initialization sequence as specified in the
 * LCD datasheet and returns a pointer to the initialized LCD handle.
 *
 * @param cols Number of columns of the LCD display.
 * @param rows Number of rows of the LCD display.
 * @param charsize Character size (5x8 dots [LCD_5x8DOTS] or 5x10 dots [LCD_5x10DOTS]).
 * @param rs GPIO pin number for the RS (Register Select) pin.
 * @param rw GPIO pin number for the RW (Read/Write) pin (255 if not used).
 * @param enable GPIO pin number for the Enable pin.
 * @param d0 GPIO pin number for data line 0.
 * @param d1 GPIO pin number for data line 1.
 * @param d2 GPIO pin number for data line 2.
 * @param d3 GPIO pin number for data line 3.
 * @param d4 GPIO pin number for data line 4.
 * @param d5 GPIO pin number for data line 5.
 * @param d6 GPIO pin number for data line 6.
 * @param d7 GPIO pin number for data line 7.
 * @param eightbitmode Boolean flag to specify if 8-bit mode is used.
 * @return LCD_Handle* Pointer to the initialized LCD handle, or NULL if initialization failed.
 */
LCD_Handle *_lcd_init(uint8_t cols, uint8_t rows, uint8_t charsize, uint8_t rs,
                      uint8_t rw, uint8_t enable, uint8_t d0, uint8_t d1,
                      uint8_t d2, uint8_t d3, uint8_t d4, uint8_t d5,
                      uint8_t d6, uint8_t d7, bool eightbitmode) {
  LCD_Handle *handle = (LCD_Handle *)malloc(sizeof(LCD_Handle));
  if (handle == NULL) {
    return NULL;
  }

  // SEE PAGE 45/46 FOR INITIALIZATION SPECIFICATION!
  // according to datasheet, we need at least 40 ms after power rises above 2.7
  // V before sending commands. Microcontroller can turn on way before 4.5 V so
  // we'll wait 50
  sleep_ms(50);

  handle->_rs_pin = rs;
  handle->_rw_pin = rw;
  handle->_enable_pin = enable;

  handle->_data_pins[0] = d0;
  handle->_data_pins[1] = d1;
  handle->_data_pins[2] = d2;
  handle->_data_pins[3] = d3;
  handle->_data_pins[4] = d4;
  handle->_data_pins[5] = d5;
  handle->_data_pins[6] = d6;
  handle->_data_pins[7] = d7;

  if (eightbitmode) {
    handle->_displayfunction = LCD_8BITMODE | LCD_1LINE | LCD_5x8DOTS;
    handle->_data_pins_mask = (0x1 << d0) | (0x1 << d1) | (0x1 << d2) |
                              (0x1 << d3) | (0x1 << d4) | (0x1 << d5) |
                              (0x1 << d6) | (0x1 << d7);
  } else {
    handle->_displayfunction = LCD_4BITMODE | LCD_1LINE | LCD_5x8DOTS;
    handle->_data_pins_mask =
        (0x1 << d0) | (0x1 << d1) | (0x1 << d2) | (0x1 << d3);
  }

  _lcd_init_pins(handle);
  _lcd_setup(handle, cols, rows, charsize);

  lcd_clear(handle);
  lcd_home(handle);

  return handle;
}

/**
 * @brief Configures the LCD display settings.
 *
 * This function sets the display parameters including the number of lines, character size,
 * and initializes the LCD in either 8-bit or 4-bit mode based on the settings provided.
 *
 * @param handle Pointer to the LCD handle.
 * @param cols Number of columns of the LCD display.
 * @param rows Number of rows of the LCD display.
 * @param charsize Character size (5x8 dots [LCD_5x8DOTS] or 5x10 dots [LCD_5x10DOTS]).
 */
void _lcd_setup(LCD_Handle *handle, uint8_t cols, uint8_t rows,
                uint8_t charsize) {
  if (rows > 1) {
    handle->_displayfunction |= LCD_2LINE;
  }
  handle->_numlines = rows;

  handle->_row_offsets[0] = 0x00;
  handle->_row_offsets[1] = 0x40;
  handle->_row_offsets[2] = 0x00 + cols;
  handle->_row_offsets[3] = 0x40 + cols;

  // for some 1 line displays you can select a 10 pixel high font
  if ((charsize != LCD_5x8DOTS) && (rows == 1)) {
    handle->_displayfunction |= LCD_5x10DOTS;
  }

  // put the LCD into 8 bit or 4 bit mode
  if (handle->_displayfunction & LCD_8BITMODE) {
    // this is according to the Hitachi HD44780 datasheet
    // page 45 figure 23

    // Send function set command sequence
    _lcd_write_8_bits(handle, LCD_FUNCTIONSET | handle->_displayfunction);
    sleep_ms(5);  // wait more than 4.1 ms

    // second try
    _lcd_write_8_bits(handle, LCD_FUNCTIONSET | handle->_displayfunction);
    sleep_ms(5);

    // third go
    _lcd_write_8_bits(handle, LCD_FUNCTIONSET | handle->_displayfunction);
    sleep_ms(5);
  } else {
    // this is according to the Hitachi HD44780 datasheet
    // page 46 figure 24

    // we start in 8bit mode, try to set 4 bit mode
    _lcd_write_4_bits(handle, 0x03);
    sleep_ms(5);  // wait min 4.1ms

    // second try
    _lcd_write_4_bits(handle, 0x03);
    sleep_ms(5);  // wait min 4.1ms

    // third go!
    _lcd_write_4_bits(handle, 0x03);
    sleep_ms(5);

    // finally, set to 4-bit interface
    _lcd_write_4_bits(handle, 0x02);
    sleep_ms(5);
  }

  // finally, set # lines, font size, etc.
  _lcd_send_command(handle, LCD_FUNCTIONSET | handle->_displayfunction);

  // turn the display on with no cursor or blinking default
  handle->_displaycontrol = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
  _lcd_send_command(handle, LCD_DISPLAYCONTROL | handle->_displaycontrol);

  // Initialize to default text direction (for romance languages)
  handle->_displaymode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
  // set the entry mode
  _lcd_send_command(handle, LCD_ENTRYMODESET | handle->_displaymode);
}

/**
 * @brief Initializes the GPIO pins used for the LCD interface.
 *
 * This function sets the direction and initial state of the GPIO pins
 * associated with the LCD data and control signals.
 *
 * @param handle Pointer to the LCD handle.
 */
void _lcd_init_pins(LCD_Handle *handle) {
  gpio_init_mask(handle->_data_pins_mask);
  gpio_set_dir_out_masked(handle->_data_pins_mask);
  gpio_put_masked(handle->_data_pins_mask, 0);

  gpio_init(handle->_rs_pin);
  gpio_set_dir(handle->_rs_pin, GPIO_OUT);
  gpio_put(handle->_rs_pin, 0);

  gpio_init(handle->_enable_pin);
  gpio_set_dir(handle->_enable_pin, GPIO_OUT);
  gpio_put(handle->_enable_pin, 0);

  if (handle->_rw_pin != 255) {
    gpio_init(handle->_rw_pin);
    gpio_set_dir(handle->_rw_pin, GPIO_OUT);
    gpio_put(handle->_rw_pin, 0);
  }
}

/**
 * @brief Deinitializes the GPIO pins used for the LCD interface.
 *
 * This function resets the GPIO pins to their default state, effectively
 * releasing them from their LCD-specific configuration.
 *
 * @param handle Pointer to the LCD handle.
 */
void _lcd_deinit_pins(LCD_Handle *handle) {
  gpio_set_function_masked(handle->_data_pins_mask, GPIO_FUNC_NULL);
  gpio_set_function(handle->_rs_pin, GPIO_FUNC_NULL);
  gpio_set_function(handle->_enable_pin, GPIO_FUNC_NULL);
  if (handle->_rw_pin != 255) {
    gpio_set_function(handle->_rw_pin, GPIO_FUNC_NULL);
  }
}

/**
 * @brief Sends a command to the LCD.
 *
 * This function sends a command byte to the LCD. It waits for the LCD to be
 * ready if the RW pin is used, sets the RS pin to command mode, and sends the
 * command in either 8-bit or 4-bit mode depending on the LCD configuration.
 *
 * @param handle Pointer to the LCD handle.
 * @param command Command byte to be sent to the LCD.
 */
void _lcd_send_command(LCD_Handle *handle, uint8_t command) {
  if (handle->_rw_pin != 255) {
    while (_lcd_busy(handle)) {
      sleep_us(3);
    }
  }
  gpio_put(handle->_rs_pin, 0);
  if (handle->_displayfunction & LCD_8BITMODE) {
    _lcd_write_8_bits(handle, command);
  } else {
    _lcd_write_4_bits(handle, command >> 4);
    _lcd_write_4_bits(handle, command);
  }
}

/**
 * @brief Sends data to the LCD.
 *
 * This function sends a data byte to the LCD. It waits for the LCD to be
 * ready if the RW pin is used, sets the RS pin to data mode, and sends the
 * data in either 8-bit or 4-bit mode depending on the LCD configuration.
 *
 * @param handle Pointer to the LCD handle.
 * @param data Data byte to be sent to the LCD.
 */
void _lcd_send_data(LCD_Handle *handle, uint8_t data) {
  if (handle->_rw_pin != 255) {
    while (_lcd_busy(handle)) {
      sleep_us(3);
    }
  }
  gpio_put(handle->_rs_pin, 1);
  if (handle->_displayfunction & LCD_8BITMODE) {
    _lcd_write_8_bits(handle, data);
  } else {
    _lcd_write_4_bits(handle, data >> 4);
    _lcd_write_4_bits(handle, data);
  }
}

/**
 * @brief Reads a command byte from the LCD.
 *
 * This function reads a command byte from the LCD. It sets the RS pin to command mode
 * and reads the command in either 8-bit or 4-bit mode depending on the LCD configuration.
 *
 * @param handle Pointer to the LCD handle.
 * @return uint8_t The command byte read from the LCD.
 */
uint8_t _lcd_read_command(LCD_Handle *handle) {
  gpio_put(handle->_rs_pin, 0);
  uint8_t command = 0;
  if (handle->_displayfunction & LCD_8BITMODE) {
    command = _lcd_read_8_bits(handle);
  } else {
    command = _lcd_read_4_bits(handle) << 4;
    command |= _lcd_read_4_bits(handle);
  }
  return command;
}

/**
 * @brief Reads a data byte from the LCD.
 *
 * This function reads a data byte from the LCD. It sets the RS pin to data mode
 * and reads the data in either 8-bit or 4-bit mode depending on the LCD configuration.
 *
 * @param handle Pointer to the LCD handle.
 * @return uint8_t The data byte read from the LCD.
 */
uint8_t _lcd_read_data(LCD_Handle *handle) {
  gpio_put(handle->_rs_pin, 1);
  uint8_t data = 0;
  if (handle->_displayfunction & LCD_8BITMODE) {
    data = _lcd_read_8_bits(handle);
  } else {
    data = _lcd_read_4_bits(handle) << 4;
    data |= _lcd_read_4_bits(handle);
  }
  return data;
}

/**
 * @brief Writes an 8-bit data byte to the LCD.
 *
 * This function sends an 8-bit data byte to the LCD by setting the data pins and
 * toggling the ENABLE pin. It also handles the RW pin if it is used.
 *
 * @param handle Pointer to the LCD handle.
 * @param data 8-bit data byte to be written to the LCD.
 */
void _lcd_write_8_bits(LCD_Handle *handle, uint8_t data) {
  if (handle->_rw_pin != 255) {
    gpio_put(handle->_rw_pin, 0);
    sleep_us(1);
    gpio_set_dir_out_masked(handle->_data_pins_mask);
  }
  gpio_put(handle->_enable_pin, 1);
  sleep_us(1);
  for (int i = 0; i < 8; i++) {
    gpio_put(handle->_data_pins[i], (data >> i) & 0x01);
  }
  sleep_us(1);
  gpio_put(handle->_enable_pin, 0);
  if (handle->_rw_pin != 255) {
    sleep_us(1);
  } else {
    sleep_us(100);
  }
}

/**
 * @brief Writes a 4-bit data nibble to the LCD.
 *
 * This function sends a 4-bit data nibble to the LCD by setting the data pins and
 * toggling the ENABLE pin. It also handles the RW pin if it is used.
 *
 * @param handle Pointer to the LCD handle.
 * @param data 4-bit data nibble to be written to the LCD.
 */
void _lcd_write_4_bits(LCD_Handle *handle, uint8_t data) {
  if (handle->_rw_pin != 255) {
    gpio_put(handle->_rw_pin, 0);
    sleep_us(1);
    gpio_set_dir_out_masked(handle->_data_pins_mask);
  }
  gpio_put(handle->_enable_pin, 1);
  sleep_us(1);
  for (int i = 0; i < 4; i++) {
    gpio_put(handle->_data_pins[i], (data >> i) & 0x01);
  }
  sleep_us(1);
  gpio_put(handle->_enable_pin, 0);
  if (handle->_rw_pin != 255) {
    sleep_us(1);
  } else {
    sleep_us(100);
  }
}

/**
 * @brief Reads an 8-bit data byte from the LCD.
 *
 * This function reads an 8-bit data byte from the LCD by setting the data pins as inputs
 * and toggling the ENABLE pin. It also handles the RW pin if it is used.
 *
 * @param handle Pointer to the LCD handle.
 * @return uint8_t The 8-bit data byte read from the LCD.
 */
uint8_t _lcd_read_8_bits(LCD_Handle *handle) {
  if (handle->_rw_pin == 255) {
    return 255;
  }
  uint8_t data = 0;
  gpio_set_dir_in_masked(handle->_data_pins_mask);
  gpio_put(handle->_rw_pin, 1);
  sleep_us(1);
  gpio_put(handle->_enable_pin, 1);
  sleep_us(1);
  for (int i = 0; i < 8; i++) {
    data |= gpio_get(handle->_data_pins[i]) << i;
  }
  gpio_put(handle->_enable_pin, 0);
  sleep_us(1);
  return data;
}

/**
 * @brief Reads a 4-bit data nibble from the LCD.
 *
 * This function reads a 4-bit data nibble from the LCD by setting the data pins as inputs
 * and toggling the ENABLE pin. It also handles the RW pin if it is used.
 *
 * @param handle Pointer to the LCD handle.
 * @return uint8_t The 4-bit data nibble read from the LCD.
 */
uint8_t _lcd_read_4_bits(LCD_Handle *handle) {
  if (handle->_rw_pin == 255) {
    return 15;
  }
  uint8_t data = 0;
  gpio_set_dir_in_masked(handle->_data_pins_mask);
  gpio_put(handle->_rw_pin, 1);
  sleep_us(1);
  gpio_put(handle->_enable_pin, 1);
  sleep_us(1);
  for (int i = 0; i < 4; i++) {
    data |= gpio_get(handle->_data_pins[i]) << i;
  }
  gpio_put(handle->_enable_pin, 0);
  sleep_us(1);
  return data;
}

/**
 * @brief Checks if the LCD is busy.
 *
 * This function checks if the LCD is busy by reading the busy flag from the LCD
 * command register. The busy flag is set when the LCD is processing a command
 * or writing data, and cleared when it is ready for new commands.
 *
 * @param handle Pointer to the LCD handle.
 * @return true if the LCD is busy, false otherwise.
 */
bool _lcd_busy(LCD_Handle *handle) { return _lcd_read_command(handle) & 0x80; }