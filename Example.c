#include <stdio.h>  // Standard input/output library

#include "src/LCD_HD44780U.h"  // Include the LCD driver header
#include "pico/stdlib.h"       // Include Pico's standard library

// Define the dimensions of the LCD
#define LCD_COLS 16  // Number of columns on the LCD
#define LCD_ROWS 2   // Number of rows on the LCD

// Define the GPIO pins for LCD control
#define LCD_RS 10  // Register select pin
#define LCD_RW 9   // Read/write pin
#define LCD_EN 8   // Enable pin

// Define the GPIO pins for LCD data (D0-D7)
#define LCD_D0 0  // Data pin D0
#define LCD_D1 1  // Data pin D1
#define LCD_D2 2  // Data pin D2
#define LCD_D3 3  // Data pin D3
#define LCD_D4 4  // Data pin D4
#define LCD_D5 5  // Data pin D5
#define LCD_D6 6  // Data pin D6
#define LCD_D7 7  // Data pin D7

#define DELAY_MS 2000

// Define a custom character (smiley face) for the LCD
uint8_t smiley[8] = {0b00000, 0b10001, 0b00000, 0b00000,
                     0b10001, 0b01110, 0b00000, 0b00000};

int main() {
  stdio_init_all();

  // Initialize the LCD in 4-bit mode
  LCD_Handle *handle_1 =
      lcd_init_4bit(LCD_COLS, LCD_ROWS, LCD_5x8DOTS, LCD_RS, LCD_RW, LCD_EN,
                    LCD_D4, LCD_D5, LCD_D6, LCD_D7);

  // Check if the LCD handle is NULL, which indicates that the initialization failed
  if (handle_1 == NULL) {
    // Print an error message to the console
    printf("Error: Failed to initialize LCD.\n");
    return 1;
  }

  // Create a custom character (smiley face) at position 0
  lcd_create_char(handle_1, 0, smiley);

  sleep_ms(DELAY_MS / 2);

  // Write "Hello Pi Pico!" to the LCD
  lcd_write_string(handle_1, "Hello Pi Pico!");
  // Write "LCD 4-bit mode." on the second line of the LCD
  lcd_write_string_at(handle_1, "LCD 4-bit mode.", 0, 1);
  // Display the custom smiley character at the first row, last column
  lcd_write_char_at(handle_1, 0, 15, 0);

  sleep_ms(DELAY_MS);

  // Deinitialize the LCD (free resources)
  handle_1 = lcd_deinit(handle_1);

  sleep_ms(DELAY_MS / 2);

  // Reinitialize the LCD in 8-bit mode
  handle_1 = lcd_init_8bit(LCD_COLS, LCD_ROWS, LCD_5x8DOTS, LCD_RS, LCD_RW,
                           LCD_EN, LCD_D0, LCD_D1, LCD_D2, LCD_D3, LCD_D4,
                           LCD_D5, LCD_D6, LCD_D7);

  // Check if the LCD handle is NULL, which indicates that the initialization failed
  if (handle_1 == NULL) {
    // Print an error message to the console
    printf("Error: Failed to initialize LCD.\n");
    return 1;
  }

  lcd_write_string(handle_1, "Hello Pi Pico!");
  lcd_write_string_at(handle_1, "LCD 8-bit mode.", 0, 1);
  lcd_write_char_at(handle_1, 0, 15, 0);

  sleep_ms(DELAY_MS);

  lcd_write_string_at(handle_1, "Clearing.......", 0, 1);

  sleep_ms(DELAY_MS);

  // Clear the display
  lcd_clear(handle_1);

  sleep_ms(DELAY_MS);

  lcd_write_string(handle_1, "Going home!");

  sleep_ms(DELAY_MS);

  // Return the cursor to the home position
  lcd_home(handle_1);

  lcd_write_string(handle_1, "Being");

  sleep_ms(DELAY_MS);

  lcd_clear(handle_1);
  lcd_home(handle_1);
  lcd_write_string(handle_1, "Display Off/On");

  sleep_ms(DELAY_MS);

  // Turn off the display
  lcd_display_off(handle_1);

  sleep_ms(DELAY_MS / 2);

  // Turn the display back on
  lcd_display_on(handle_1);

  sleep_ms(DELAY_MS / 2);

  lcd_clear(handle_1);
  lcd_home(handle_1);
  lcd_write_string(handle_1, "Cursor On...");

  // Turn the cursor on
  lcd_cursor_on(handle_1);

  sleep_ms(DELAY_MS / 2);
  lcd_write_string_at(handle_1, "Blinking...", 0, 1);

  // Enable cursor blinking
  lcd_blink_on(handle_1);

  sleep_ms(DELAY_MS);

  lcd_clear(handle_1);
  lcd_home(handle_1);
  lcd_write_string(handle_1, "Cursor Off...");

  // Turn off the cursor and blink mode
  lcd_cursor_off(handle_1);
  lcd_blink_off(handle_1);

  sleep_ms(DELAY_MS);

  lcd_clear(handle_1);
  lcd_home(handle_1);
  lcd_write_string(handle_1, "Scrolling content...");
  lcd_write_string_at(handle_1, "To left", 0, 1);

  sleep_ms(DELAY_MS / 2);

  // Scroll the display to the left five times
  for (size_t i = 0; i < 5; i++) {
    lcd_scroll_display_left(handle_1);
    sleep_ms(DELAY_MS / 5);
  }

  sleep_ms(DELAY_MS / 2);

  lcd_write_string_at(handle_1, "To right", 0, 1);

  // Scroll the display to the right five times
  for (size_t i = 0; i < 5; i++) {
    lcd_scroll_display_right(handle_1);
    sleep_ms(DELAY_MS / 5);
  }

  sleep_ms(DELAY_MS / 2);

  lcd_clear(handle_1);
  lcd_home(handle_1);

  // Test text direction change
  lcd_right_to_left(handle_1);
  lcd_write_string_at(handle_1, "Right to left...", 15, 0);
  lcd_left_to_right(handle_1);
  lcd_write_string_at(handle_1, "Left to right...", 0, 1);

  sleep_ms(2 * DELAY_MS);

  lcd_clear(handle_1);
  lcd_home(handle_1);
  lcd_write_string(handle_1, "          Autoscroll");
  // Turn autoscrol on
  lcd_autoscroll_on(handle_1);
  lcd_set_cursor(handle_1, 10, 1);
  // Write characters one by one with a delay
  sleep_ms(DELAY_MS / 5);
  lcd_write_char(handle_1, 'e');
  sleep_ms(DELAY_MS / 5);
  lcd_write_char(handle_1, 'n');
  sleep_ms(DELAY_MS / 5);
  lcd_write_char(handle_1, 'a');
  sleep_ms(DELAY_MS / 5);
  lcd_write_char(handle_1, 'b');
  sleep_ms(DELAY_MS / 5);
  lcd_write_char(handle_1, 'l');
  sleep_ms(DELAY_MS / 5);
  lcd_write_char(handle_1, 'e');
  sleep_ms(DELAY_MS / 5);
  lcd_write_char(handle_1, 'd');
  sleep_ms(DELAY_MS / 5);
  lcd_write_char(handle_1, '.');
  sleep_ms(DELAY_MS / 5);
  lcd_write_char(handle_1, '.');
  sleep_ms(DELAY_MS / 5);
  lcd_write_char(handle_1, '.');
  sleep_ms(DELAY_MS / 5);

  sleep_ms(DELAY_MS);

  // Disable autoscroll and display "Bye bye!"
  lcd_autoscroll_off(handle_1);
  lcd_clear(handle_1);
  lcd_home(handle_1);
  lcd_write_string(handle_1, "Bye bye!");

  sleep_ms(DELAY_MS);

  // Deinitialize the LCD (free resources)
  handle_1 = lcd_deinit(handle_1);
}
