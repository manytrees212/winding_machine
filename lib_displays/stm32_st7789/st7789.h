#ifndef INC_GMT020_02_H_
#define INC_GMT020_02_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "fonts.h"
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

// LCD Command Definitions
#define ST7789_SWRESET  0x01  //Software reset
#define ST7789_SLPOUT   0x11  //Sleep out
#define ST7789_COLMOD   0x3A  //Interface pixel format || Interface format
#define ST7789_MADCTL   0x36  //Memory data access control
#define ST7789_CASET    0x2A  //Column address set
#define ST7789_RASET    0x2B  //Row address set
#define ST7789_INVON    0x21  //Display inversion ON
#define ST7789_NORON    0x13  //Partial off (Normal mode)
#define ST7789_RAMWR    0x2C  //Memory write || Write data
#define ST7789_DISPON   0x29  //Display on
#define ST7789_PORCTRL  0xB2  //Porch settings (blanking time between frames)
#define ST7789_GCTRL    0xB7  //Gate control
#define ST7789_VCOMS    0xBB  //Controls the voltage level
#define ST7789_LCMCTRL  0xC0  //LCM control
#define ST7789_VDVVRHEN 0xC2  //Power control
#define ST7789_VRHS     0xC3  //The highest gate voltage
#define ST7789_VDVS     0xC4  //The reference for the data signal amplitude
#define ST7789_FRCTRL2  0xC6  //Frame Rate Control in Normal Mode
#define ST7789_PWCTRL1  0xD0  //Configuring the internal DC-DC converter
#define ST7789_PVGAMCTRL 0xE0 //Positive gamma
#define ST7789_NVGAMCTRL 0xE1 //Negative gamma
#define ST7789_VSCSAD   0x37  //Vertical Scroll Start Address of RAM
#define ST7789_VSCRDEF  0x33  //Vertical Scrolling Definition

#define ST7789_XSTART   0x0000
#define ST7789_YSTART   0x0000
//#define ST7789_YOFFSET  80
//#define ST7789_XOFFSET  80

#define LCD_WIDTH       ((uint16_t)240)
#define LCD_HEIGHT      ((uint16_t)320)
#define LCD_SIZE        ((uint32_t)LCD_WIDTH * LCD_HEIGHT)
#define VISIBLE_HEIGHT  ((uint16_t)240)
#define VERTICAL_OFFSET ((uint16_t)80)

//Color mode
#define RGB565 0x55

// RGB565 Colors
#define BLACK   0x0000
#define WHITE   0xFFFF
#define RED     0xF800
#define GREEN   0x07E0
#define BLUE    0x001F
#define CYAN    0x07FF
#define YELLOW  0xFFC0
#define PURPLE  0xF81F
#define PINK    0xD252

//Memory data access control
#define ST7789_MADCTL_MY    0x80 //bit D7 mirror Y
#define ST7789_MADCTL_MX	0x40 //bit D6 mirror X
#define ST7789_MADCTL_MV	0x20 //bit D5 exchange XY
#define ST7789_MADCTL_ML    0x10 //bit D4 refresh lines (top to bottom if 0)
#define ST7789_MADCTL_BGR	0x08 //bit D3 mirror color
#define ST7789_MADCTL_MH	0x04 //bit D2 latch data order (left to right if 0)

typedef enum{
    portrait=0,
    landscape,
    portrait_mirror,
    landscape_mirror
}ScreenOrientation;

typedef struct{
    uint8_t my;  // mirror Y
    uint8_t mx;  // mirror X
    uint8_t mv;  // exchange XY
    uint8_t ml;  // refresh bottom-top
    uint8_t bgr; // BGR order (1 for ST7789)
    uint8_t mh;  // latch right-left
}OrientationConf;

typedef struct{
    uint16_t x;
    uint16_t y;
}PixelCoord;

typedef enum{
    Disp_st7789_OK,
    Disp_st7789_ERROR
}DisplayStatus;

typedef struct{
    void (*spi_transmit)(const uint8_t* data, uint16_t len);
    void (*set_cs_pin)(bool state);
    void (*set_dc_pin)(bool state);
    void (*set_rst_pin)();
    void (*set_blk_pin)(uint8_t);
    void (*set_delay)(uint16_t);
}ST7789_HW_Interface;

// LCD Driver API
typedef struct{
    ST7789_HW_Interface hw_interface;
    ScreenOrientation orientation;
    uint8_t brightness_percent;
    uint16_t width;
    uint16_t height;
    uint32_t size;
    bool initialized;
}ST7789_display;

DisplayStatus ST7789_init(ST7789_display* display, const ST7789_HW_Interface* hw);

ScreenOrientation ST7789_get_screen_orientation(const ST7789_display* display);
void ST7789_set_screen_orientation(ST7789_display* display, const ScreenOrientation orientation);
void ST7789_set_brightness(ST7789_display* display, const uint16_t brightness);

void ST7789_draw_rectangle(const ST7789_display* display,
                           const PixelCoord left_top,
                           const PixelCoord right_bottom,
                           const uint16_t color);

void ST7789_clear_screen(const ST7789_display* display, const uint16_t color);

void ST7789_draw_char(const ST7789_display* display,
                      const PixelCoord pixel,
                      const uint16_t color,
                      const FontDef font,
                      const char letter);

void ST7789_draw_string(const ST7789_display* display,
                        const uint16_t x,
                        const uint16_t y,
                        const uint16_t color,
                        const FontSize font_size,
                        const char *str);

#ifdef __cplusplus
}
#endif

#endif /* INC_GMT020_02_H_ */
