#ifndef LCD_1602_H
#define LCD_1602_H

#include <stdint.h>
#include <stdbool.h>

#define LCD1602_ADDRESS 0x27;

//pins of PCF8574T (expanded board)
#define PIN_RS     0x01  // 0=command, 1=data
#define PIN_RW     0x02  // 0=write, 1=read
#define PIN_EN     0x04  // Enable (strobe)
#define PIN_BL     0x08  // Backlight
#define PIN_D4     0x10  // Data
#define PIN_D5     0x20  // Data
#define PIN_D6     0x40  // Data
#define PIN_D7     0x80  // Data

#define COMMAND 0x01
#define DATA 0x00

// LCD Command Definitions
#define LCD1602_CLEARDISPLAY    0x01
#define LCD1602_RETURNHOME      0x02

#define LCD1602_ENTRYMODESET    0x04
#define CURSOR_INCREMENT        (LCD1602_ENTRYMODESET | 0x02)
#define CURSOR_DECREMENT        (LCD1602_ENTRYMODESET | 0x00)

#define LCD1602_DISPLAYCONTROL  0x08
#define DISP_TURN_ON            (LCD1602_DISPLAYCONTROL | 0x04)
#define DISP_TURN_OFF           (LCD1602_DISPLAYCONTROL | 0x00)
#define CURSOR_ON               (DISP_TURN_ON  | 0x02)
#define CURSOR_OFF              (DISP_TURN_OFF | 0x00)
#define CURSOR_BLINK            (CURSOR_ON | 0x01)
#define CURSOR_NON_BLINK        (CURSOR_ON | 0x00)

#define LCD1602_CURSORSHIFT     0x10
#define SHIFT_CURSOR_LEFT       (LCD1602_CURSORSHIFT | 0x00)
#define SHIFT_CURSOR_RIGHT      (LCD1602_CURSORSHIFT | 0x04)
#define SHIFT_DISPLAY_LEFT      (LCD1602_CURSORSHIFT | 0x08)
#define SHIFT_DISPLAY_RIGHT     (LCD1602_CURSORSHIFT | 0x0C)

#define LCD1602_FUNCTIONSET     0x20
#define NIBBLE_8BITS            (LCD1602_FUNCTIONSET | 0x10)
#define NIBBLE_4BITS            (LCD1602_FUNCTIONSET | 0x00)
#define LINE_NUMBER             (LCD1602_FUNCTIONSET | 0x08)
#define LCD1602_CHAR_FONT_5x8   (LCD1602_FUNCTIONSET | 0x04)
#define LCD1602_CHAR_FONT_5x10  (LCD1602_FUNCTIONSET | 0x00)

// Common combined commands
#define LCD1602_INIT_4BIT_2LINE  0x28
#define LCD1602_INIT_8BIT_2LINE  0x38

#define LCD1602_SETCGRAMADDR    0x40
#define LCD1602_SETDDRAMADDR    0x80

#define LCD1602_ROW1_START      0x00
#define LCD1602_ROW2_START      0x40

#define LCD1602_CURSOR_POS(row, col) \
        (LCD1602_SETDDRAMADDR | ((row == 0 ? 0x00 : 0x40) + col))

typedef enum{
    LCD1602_OK,
    LCD1602_ERROR
}DisplayStatus1602;

typedef struct{
    void (*i2c_send_data)(const uint8_t slave_addr,
                          const uint8_t data,
                          const uint16_t len
                          );
    void (*set_delay_ms)(const uint16_t);
}LCD1602_HW_Interface;

//LCD Driver API
typedef struct{
    LCD1602_HW_Interface hw_interface;
    uint8_t address;
    bool backlight;
}LCD1602_display;

DisplayStatus1602 LCD1602_display_init(LCD1602_display *display,
                                   LCD1602_HW_Interface* hw);

void LCD1602_clear_screen(const LCD1602_display *display);

void LCD1602_set_cursor(const LCD1602_display *display,
                        const uint8_t row, const uint8_t col);

void LCD1602_draw_char(const LCD1602_display *display,
                       const char letter);

void LCD1602_draw_string(const LCD1602_display *display,
                         const char* data,
                         const uint16_t len);

void set_backlight(LCD1602_display *display);

void LCD1602_turn_on(const LCD1602_display *display,
                     bool cursor_on, bool blink_on);

void LCD1602_turn_off(const LCD1602_display *display);

#endif //LCD_1602_H
