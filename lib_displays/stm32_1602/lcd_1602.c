#include "lcd_1602.h"
#include <stdint.h>

static void send_command(const LCD1602_display *display, const uint8_t cmd);
static void send_data(const LCD1602_display *display, const uint8_t data);
static void lcd_send_byte(const LCD1602_display *display,
                             const uint8_t byte, const uint8_t is_command);

static void lcd_send_nibble(const LCD1602_display *display,
                            const uint8_t nibble,
                            const uint8_t is_command);

static void test_lcd1602(const LCD1602_display *display);

DisplayStatus1602 LCD1602_display_init(LCD1602_display *display,
                                   LCD1602_HW_Interface* hw){
    if(!display || !hw){
        return LCD1602_ERROR;
    }

    display->hw_interface = *hw;
    display->address = LCD1602_ADDRESS;
    display->backlight = true;

    display->hw_interface.set_delay_ms(50);

    lcd_send_nibble(display, NIBBLE_8BITS, true);
    display->hw_interface.set_delay_ms(5);
    lcd_send_nibble(display, NIBBLE_8BITS, true);
    display->hw_interface.set_delay_ms(1);
    lcd_send_nibble(display, NIBBLE_8BITS, true);
    display->hw_interface.set_delay_ms(1);

    lcd_send_nibble(display, NIBBLE_4BITS, true);
    display->hw_interface.set_delay_ms(1);

    // Function set: 4-bit, 2 lines, 5x8 font
    send_command(display, LCD1602_INIT_4BIT_2LINE);
    display->hw_interface.set_delay_ms(1);

    // Display off during configuration
    LCD1602_turn_off(display);

    // Clear display
    LCD1602_clear_screen(display);

    // Entry mode: increment cursor, no shift
    send_command(display, CURSOR_INCREMENT);
    display->hw_interface.set_delay_ms(1);

    // Display on (cursor off by default)
    LCD1602_turn_on(display, false, false);
    test_lcd1602(display);
    return LCD1602_OK;
}

void send_command(const LCD1602_display *display, const uint8_t cmd){
    lcd_send_byte(display, cmd, COMMAND);
}

void send_data(const LCD1602_display *display, const uint8_t data){
    lcd_send_byte(display, data, DATA);
}

void lcd_send_byte(const LCD1602_display *display,
                   const uint8_t byte, const uint8_t is_command){
    uint8_t high_nibble = byte & 0xF0;
    uint8_t low_nibble = (byte<<4) & 0xF0;
    lcd_send_nibble(display, high_nibble, is_command);
    lcd_send_nibble(display, low_nibble, is_command);
}

//set up data format [D7 D6 D5 D4 BL EN RW RS]
void lcd_send_nibble(const LCD1602_display *display,
                            const uint8_t nibble, const uint8_t is_command){

    uint8_t data = nibble;  // D4-D7

    if (is_command) {
        data &= ~PIN_RS;// RS=0 for command
    } else {
        data |= PIN_RS; // RS=1 for data
    }

    if (display->backlight) {
        data |= PIN_BL; // BL=1 backlight ON
    } else {
        data &= ~PIN_BL;// BL=0 backlight OFF
    }

    // Step 1: Clear EN
    data &= ~PIN_EN;
    display->hw_interface.i2c_send_data(display->address, data, 1);
    display->hw_interface.set_delay_ms(1);

    // Step 2: Set EN
    data |= PIN_EN;
    display->hw_interface.i2c_send_data(display->address, data, 1);
    display->hw_interface.set_delay_ms(1);

    // Step 3: Clear EN
    data &= ~PIN_EN;
    display->hw_interface.i2c_send_data(display->address, data, 1);
    display->hw_interface.set_delay_ms(2);
}

void LCD1602_clear_screen(const LCD1602_display *display){
    send_command(display, LCD1602_CLEARDISPLAY);
    display->hw_interface.set_delay_ms(2);
}

void set_backlight(LCD1602_display *display){

}

void LCD1602_turn_on(const LCD1602_display *display, bool cursor_on, bool blink_on){
    uint8_t cmd = DISP_TURN_ON;
    if(cursor_on){
        cmd |= CURSOR_ON;
    }
    if(blink_on){
        cmd |= CURSOR_BLINK;
    }
    send_command(display, cmd);
    display->hw_interface.set_delay_ms(1);
}

void LCD1602_turn_off(const LCD1602_display *display){
    send_command(display, DISP_TURN_OFF);
    display->hw_interface.set_delay_ms(1);
}

void LCD1602_set_cursor(const LCD1602_display *display,
                        const uint8_t row, const uint8_t col){
    uint8_t position = LCD1602_CURSOR_POS(row, col);
    send_command(display, position);
    display->hw_interface.set_delay_ms(1);
}

void LCD1602_draw_char(const LCD1602_display *display, const char letter){
    send_data(display, (uint8_t)letter);
}

void LCD1602_draw_string(const LCD1602_display *display,
                         const char* data, const uint16_t len){
    for(uint16_t i = 0; i < len && data[i] != '\0'; i++){
        send_data(display, (uint8_t)data[i]);
    }
}

void test_lcd1602(const LCD1602_display *display){
    LCD1602_set_cursor(display, 0, 0);
    LCD1602_draw_string(display, "HELLO DISPLAY!", 14);
    LCD1602_set_cursor(display, 1, 0);
    LCD1602_draw_string(display, "I2C WORKS!", 10);
}
