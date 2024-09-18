/*
 * SPDX-FileCopyrightText: 2024 Jozef Kromka <jozef.kromka22@gmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

// ########################################################################## //
//                                                                            //
//             Raspberry Pi Pico LCD HD44780U library header file             //
//                                                                            //
// ########################################################################## //

#ifndef __LCD_HD44780U__
#define __LCD_HD44780U__

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

// ########################################################################## //
//                                                                            //
//                            Constant Definitions                            //
//                                                                            //
// ########################################################################## //

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

// ########################################################################## //
//                                                                            //
//                            Structure definition                            //
//                                                                            //
// ########################################################################## //

// Define a structure for the HD44780U LCD controller.
typedef struct LCD_HD44780U {
  // Pin to control Register Select (RS):
  // LOW: command mode. HIGH: data mode (sending characters).
  uint8_t _rs_pin;
  // Pin to control Read/Write (R/W):
  // LOW: write to the LCD. HIGH: read from the LCD.
  uint8_t _rw_pin;
  // Pin to control the Enable signal:
  // A HIGH pulse activates the LCD to read or write data.
  uint8_t _enable_pin;
  // Array to hold the data pins (D0-D7)
  uint8_t _data_pins[8];
  // A mask for the data pins
  uint32_t _data_pins_mask;
  // LCD display function settings
  uint8_t _displayfunction;
  // LCD display control settings
  uint8_t _displaycontrol;
  // LCD display mode settings
  uint8_t _displaymode;
  // Number of lines on the LCD:
  // Typically 1, 2, or 4 depending on the specific LCD module.
  uint8_t _numlines;
  // Array to store the row offsets
  uint8_t _row_offsets[4];
} LCD_Handle;

// ########################################################################## //
//                                                                            //
//                        Public functions definition                         //
//                                                                            //
// ########################################################################## //

LCD_Handle *lcd_init_8bit(uint8_t cols, uint8_t rows, uint8_t charsize,
                          uint8_t rs, uint8_t rw, uint8_t enable, uint8_t d0,
                          uint8_t d1, uint8_t d2, uint8_t d3, uint8_t d4,
                          uint8_t d5, uint8_t d6, uint8_t d7);
LCD_Handle *lcd_init_4bit(uint8_t cols, uint8_t rows, uint8_t charsize,
                          uint8_t rs, uint8_t rw, uint8_t enable, uint8_t d4,
                          uint8_t d5, uint8_t d6, uint8_t d7);

LCD_Handle *lcd_deinit(LCD_Handle *handle);

void lcd_clear(LCD_Handle *handle);
void lcd_home(LCD_Handle *handle);

void lcd_display_off(LCD_Handle *handle);
void lcd_display_on(LCD_Handle *handle);
void lcd_blink_off(LCD_Handle *handle);
void lcd_blink_on(LCD_Handle *handle);
void lcd_cursor_off(LCD_Handle *handle);
void lcd_cursor_on(LCD_Handle *handle);
void lcd_scroll_display_left(LCD_Handle *handle);
void lcd_scroll_display_right(LCD_Handle *handle);
void lcd_left_to_right(LCD_Handle *handle);
void lcd_right_to_left(LCD_Handle *handle);
void lcd_autoscroll_off(LCD_Handle *handle);
void lcd_autoscroll_on(LCD_Handle *handle);

void lcd_set_cursor(LCD_Handle *handle, uint8_t col, uint8_t row);
void lcd_write_char(LCD_Handle *handle, char symbol);
void lcd_write_string(LCD_Handle *handle, char *text);
void lcd_write_char_at(LCD_Handle *handle, char symbol, uint8_t col, uint8_t row);
void lcd_write_string_at(LCD_Handle *handle, char *text, uint8_t col, uint8_t row);
void lcd_create_char(LCD_Handle *handle, uint8_t num, uint8_t *data);

#endif