#include "fonts.h"
#include "st7789.h"

static void ST7789_hardware_reset(ST7789_display* display);

static void ST7789_write_data(const ST7789_display *display,
                              const uint8_t *data, const uint16_t size);

static void ST7789_write_16bit(const ST7789_display* display, const uint16_t *data);
static void ST7789_write_command(const ST7789_display *display,const uint8_t command);

static void ST7789_set_active_window(const ST7789_display* display,
                                     const PixelCoord left_top,
                                     const PixelCoord right_bottom);

static void ST7789_set_backlight_start();

static uint8_t get_configure(const OrientationConf* orientation);

static void ST7789_draw_pixel(const ST7789_display* display,
                              const PixelCoord pixel_coord,
                              const uint16_t color);

static void test_orientation_with_box(  ST7789_display* display,
                                        const ScreenOrientation orientation,
                                        const char* name,
                                        const uint16_t color);


DisplayStatus ST7789_init(ST7789_display* display, const ST7789_HW_Interface* hw){

    if(!display || !hw){
        return Disp_st7789_ERROR;
    }

    if( !hw->spi_transmit || !hw->set_cs_pin || !hw->set_dc_pin
        || !hw->set_blk_pin || !hw->set_rst_pin || !hw->set_delay){
        return Disp_st7789_ERROR;
    }

    memset(display, 0, sizeof(ST7789_display));

    display->hw_interface = *hw;
    display->width = LCD_WIDTH;
    display->height = LCD_HEIGHT;
    display->size = LCD_SIZE;

    //Reset(software)
    ST7789_write_command(display, ST7789_SWRESET);
    display->hw_interface.set_delay(150);

    // Sleep out
    ST7789_write_command(display, ST7789_SLPOUT);
    display->hw_interface.set_delay(150);

    //Color mode 16bit: RGB565
    ST7789_write_command(display, ST7789_COLMOD);
    uint8_t rgb = RGB565;
    ST7789_write_data(display, &rgb, 1);
    display->hw_interface.set_delay(10);

    //Memory access data control
    ST7789_set_screen_orientation(display, portrait);

    //Porch settings (blanking time between frames)
    ST7789_write_command(display, ST7789_PORCTRL);
    uint8_t porch_param[] ={0x0C,/*back_porch_normal*/
                            0x0C,/*front_porch_normal*/
                            0x00,/*separate_porch_disable*/
                            0x33,/*back_frnt_porch_idle*/
                            0x33 /*back_frnt_porch_partial*/
                           };
    uint16_t param_number = sizeof(porch_param)/sizeof(porch_param[0]);
    ST7789_write_data(display, &porch_param[0], param_number);

    //Gate control: sets the timing and mode
    //for the vertical scanning of the display
    ST7789_write_command(display, ST7789_GCTRL);
    uint8_t gctrl = 0x35; // by hardare default
    ST7789_write_data(display, &gctrl, 1);

    // VCOMS setting:
    // controls the voltage level for the display's common electrode
    ST7789_write_command(display, ST7789_VCOMS);
    uint8_t vcoms = 0x19; //by default 0x20, 0x19 based on common using
    ST7789_write_data(display, &vcoms, 1);

    // LCM control
    //Configures how the 16/18/24-bit RGB data from the controller
    //is mapped to the physical panel's pixel inputs.
    ST7789_write_command(display, ST7789_LCMCTRL);
    uint8_t lcmctrl = 0x2C; // by hardare default
    ST7789_write_data(display, &lcmctrl, 1);

    // Power control
    //Ignore the voltages stored in permanent memory (NVM)
    //Enables manual control of the VDV and VRH below
    ST7789_write_command(display, ST7789_VDVVRHEN);
    uint8_t vdvvrhen[] = {0x01, 0xFF}; // by hardare default
    param_number = sizeof(vdvvrhen)/sizeof(vdvvrhen[0]);
    ST7789_write_data(display, &vdvvrhen[0], param_number);

    //VRHS: the highest gate voltage
    //Affects contrast and pixel response
    ST7789_write_command(display, ST7789_VRHS);
    uint8_t vrhs = 0x12;
    ST7789_write_data(display, &vrhs, 1);

    //VDVS: the reference for the data signal amplitude
    //Affects color depth and sharpness
    ST7789_write_command(display, ST7789_VDVS);
    uint8_t vdvs = 0x20;
    ST7789_write_data(display, &vdvs, 1);

    // Frame Rate Control in Normal Mode
    ST7789_write_command(display, ST7789_FRCTRL2);
    uint8_t frctrl2 = 0x0F;
    ST7789_write_data(display, &frctrl2, 1);

    //Power control 1
    //Configurating the internal DC-DC converter
    ST7789_write_command(display, ST7789_PWCTRL1);
    uint8_t pwctrl1[] = {0xA4, 0xA1}; // by hardare default
    param_number = sizeof(pwctrl1)/sizeof(pwctrl1[0]);
    ST7789_write_data(display, &pwctrl1[0], param_number);

    // Positive gamma
    ST7789_write_command(display, ST7789_PVGAMCTRL);
    uint8_t pvgamctrl[] = {0xD0, 0x04, 0x0D, 0x11, 0x13, 0x2B, 0x3F, 0x54, 0x4C, 0x18, 0x0D, 0x0B, 0x1F, 0x23};
    param_number = sizeof(pvgamctrl)/sizeof(pvgamctrl[0]);
    ST7789_write_data(display, &pvgamctrl[0], param_number);

    // Negative gamma
    ST7789_write_command(display, ST7789_NVGAMCTRL);
    uint8_t nvgamctrl[] = {0xD0, 0x04, 0x0C, 0x11, 0x13, 0x2C, 0x3F, 0x44, 0x51, 0x2F, 0x1F, 0x1F, 0x20, 0x23};
    param_number = sizeof(nvgamctrl)/sizeof(nvgamctrl[0]);
    ST7789_write_data(display, &nvgamctrl[0], param_number);

    ST7789_write_command(display, ST7789_INVON);
    display->hw_interface.set_delay(10);

    //Normal mode
    ST7789_write_command(display, ST7789_NORON);
    display->hw_interface.set_delay(10);

    // Display on
    ST7789_write_command(display, ST7789_DISPON);
    display->hw_interface.set_delay(10);

    ST7789_set_brightness(display, 50);

    // Filling in with BLUE
    ST7789_clear_screen(display, BLUE);
    display->hw_interface.set_delay(10);
#if 0
    test_orientation_with_box(display, landscape, "Landscape", GREEN);
    display->hw_interface.set_delay(20);

    test_orientation_with_box(display, portrait_mirror, "Portrait Mirror", BLUE);
    display->hw_interface.set_delay(20);
    test_orientation_with_box(display, landscape_mirror, "Landscape Mirror", YELLOW);
    display->hw_interface.set_delay(20);
    test_orientation_with_box(display, portrait, "Portrait", RED);
#endif
    return Disp_st7789_OK;
}

void ST7789_write_command(const ST7789_display* display, const uint8_t command){
    uint8_t cmd = command;
    display->hw_interface.set_dc_pin(0);
    display->hw_interface.spi_transmit(&cmd, 1);
}

void ST7789_write_data(const ST7789_display* display, const uint8_t *data, const uint16_t size) {
    if(data==0 || size==0){
        return;
    }
    display->hw_interface.set_dc_pin(1);
    display->hw_interface.spi_transmit(data, size);
}

void ST7789_write_16bit(const ST7789_display* display, const uint16_t *data){
    uint8_t bufferA = (*data) >> 8;
    uint8_t bufferB = (*data) & 0xFF;

    ST7789_write_data(display, &bufferA, 1);
    ST7789_write_data(display, &bufferB, 1);
}

void ST7789_set_active_window(const ST7789_display* display, const PixelCoord left_top, const PixelCoord right_bottom){

    uint16_t x_start = left_top.x;
    uint16_t x_end = right_bottom.x;
    uint16_t y_start = left_top.y;
    uint16_t y_end = right_bottom.y;

    if(x_start > x_end){
        x_start=right_bottom.x;
        x_end=left_top.x;
    }

    if(y_start > y_end){
        y_start = right_bottom.y;
        y_end = left_top.y;
    }

    if(x_end > LCD_WIDTH) {
        x_end = LCD_WIDTH-1;
    }

    if(y_end > VISIBLE_HEIGHT) {
        y_end = VISIBLE_HEIGHT-1;
    }

    // CASET sets column range
    ST7789_write_command(display, ST7789_CASET);
    ST7789_write_16bit(display, &x_start);
    ST7789_write_16bit(display, &x_end);

    // RASET sets raw range
    ST7789_write_command(display, ST7789_RASET);
    ST7789_write_16bit(display, &y_start);
    ST7789_write_16bit(display, &y_end);

    // RAMWR writes window from MCU to frame memory
    ST7789_write_command(display, ST7789_RAMWR);
}

static void ST7789_draw_pixel(const ST7789_display* display, const PixelCoord pixel_cood, const uint16_t color){
    ST7789_set_active_window(display, pixel_cood, pixel_cood);
    uint16_t c = color;
    ST7789_write_16bit(display, &c);
}

void ST7789_clear_screen(const ST7789_display* display, const uint16_t color){
    PixelCoord pixel_start = {ST7789_XSTART, ST7789_YSTART};
    PixelCoord pixel_end = {ST7789_XSTART+LCD_WIDTH-1, ST7789_YSTART+LCD_HEIGHT-1};
    ST7789_draw_rectangle(display, pixel_start, pixel_end, color);
}

void ST7789_draw_rectangle(const ST7789_display *display,
                           const PixelCoord left_top, const PixelCoord right_bottom, const uint16_t color){
    ST7789_set_active_window(display, left_top, right_bottom);
    uint16_t c = color;
    for (uint32_t y = 0; y < right_bottom.y; ++y){
        for (uint32_t x = 0; x < right_bottom.x; ++x)
            ST7789_write_16bit(display, &c);
    }
}

void ST7789_draw_char(const ST7789_display* display, const PixelCoord pixel,
                        const uint16_t color, const FontDef font, const char letter){

    if((letter < 32) || (letter > 255)) return;
    uint8_t letter_idx = letter - 32;

    for (uint8_t col = 0; col < font.col_number; col++) {
        uint16_t col_bits = 0;

        if (font.font_size == fnt13x8){
            col_bits = font.font_data.fnt13x8[letter_idx][col];
        } else {
            col_bits = font.font_data.fnt20x12[letter_idx][col];
            col_bits = col_bits >> 4;
        }

        for (uint8_t row = 0; row < font.row_number; row++) {
            if (col_bits & (1 << row)) {
                PixelCoord current_pixel = {pixel.x+(font.row_number - 1 - row), pixel.y+col};
                ST7789_draw_pixel(display, current_pixel, color);
            }
        }
    }
}

void ST7789_draw_string(const ST7789_display* display,
                        const uint16_t x, const uint16_t y, const uint16_t color,
                                    const FontSize font_size, const char* string){

    PixelCoord current_pixel = {x, y};
    FontDef current_font;
    switch(font_size){
        case fnt13x8:
            current_font.font_size=font_size;
            current_font.col_number=13;
            current_font.row_number=8;
            current_font.font_data.fnt13x8=font13x8;
            break;
        case fnt20x12:
            current_font.font_size=font_size;
            current_font.col_number=20;
            current_font.row_number=12;
            current_font.font_data.fnt20x12=font20x12;
            break;
        default:
            return;
    }

    uint8_t step_x = current_font.col_number*2/3;
    uint8_t step_y = current_font.row_number+2;

    for (uint8_t i = 0; string[i] != '\0'; i++) {
        if(current_pixel.x >= LCD_WIDTH){
            current_pixel.y+=step_y;
            current_pixel.x = 0;

            if(current_pixel.y >= LCD_HEIGHT){
                current_pixel.y = 0;
            }
        }
        ST7789_draw_char(display, current_pixel, color, current_font, string[i]);
        current_pixel.x += step_x;
    }
}

void hard_ware_reset(const ST7789_display* display){
    display->hw_interface.set_rst_pin();
}

uint8_t get_configure(const OrientationConf* orientation){

    uint8_t result=0;
    if(orientation->my)    result |= ST7789_MADCTL_MY;
    if(orientation->mx)    result |= ST7789_MADCTL_MX;
    if(orientation->mv)    result |= ST7789_MADCTL_MV;
    if(orientation->ml)    result |= ST7789_MADCTL_ML;
    if(orientation->bgr)   result |= ST7789_MADCTL_BGR;
    if(orientation->mh)    result |= ST7789_MADCTL_MH;
    return result;
}

void ST7789_set_screen_orientation(ST7789_display* display, const ScreenOrientation orientation){

    uint8_t madctl = 0;

    //setting up the scrolling defenitions for ST7789_VSCRDEF
    uint16_t vscrdef[]={0, LCD_HEIGHT, 0};//TFA, VSA, BFA

    //setting up start scroll for ST7789_VSCSAD
    uint16_t vscsad = 0;

    switch(orientation){
        case portrait:
            OrientationConf port = {0, 0, 0, 0, 0, 0};
            madctl = get_configure(&port);
            break;
        case landscape:
            OrientationConf landscp = {0, 1, 1, 0, 0, 0};
            madctl = get_configure(&landscp);
            break;
        case portrait_mirror:
            OrientationConf port_m = {1, 1, 0, 0, 0, 0};
            madctl = get_configure(&port_m);
            break;
        case landscape_mirror:
            OrientationConf landscp_m= {1, 0, 1, 0, 0, 0};
            madctl = get_configure(&landscp_m);
            break;
    }

    ST7789_write_command(display, ST7789_MADCTL);
    ST7789_write_data(display, &madctl, 1);
    display->hw_interface.set_delay(10);

    if(orientation == portrait_mirror || orientation == landscape_mirror){
        vscrdef[0] = 0;
        vscrdef[1] = VISIBLE_HEIGHT;
        vscrdef[2] = 0;
        vscsad = VERTICAL_OFFSET;
    }

    ST7789_write_command(display, ST7789_VSCRDEF);
    ST7789_write_16bit(display, &vscrdef[0]);
    ST7789_write_16bit(display, &vscrdef[1]);
    ST7789_write_16bit(display, &vscrdef[2]);
    display->hw_interface.set_delay(10);

    ST7789_write_command(display, ST7789_VSCSAD);
    ST7789_write_16bit(display, &vscsad);
    display->hw_interface.set_delay(10);

    ST7789_clear_screen(display, BLACK);
    display->hw_interface.set_delay(50);

    display->orientation = orientation;
}

ScreenOrientation ST7789_get_current_orientation(const ST7789_display* display){
    return display->orientation;
}

void ST7789_set_brightness(ST7789_display* display, const uint16_t brightness_percent) {
    display->hw_interface.set_blk_pin(brightness_percent);
    display->brightness_percent = brightness_percent;
}

void test_orientation_with_box(ST7789_display* display,
                               const ScreenOrientation orientation,
                                            const char* name, const uint16_t color) {

    ST7789_set_screen_orientation(display, orientation);
    display->hw_interface.set_delay(500);

    // Draw a colored box in the center
    PixelCoord box_start = {70, 70};
    PixelCoord box_end = {169, 169};  // 100x100 box

    ST7789_draw_rectangle(display, box_start, box_end, color);

    // Draw text at top-left (should move based on orientation)
    ST7789_draw_string(display, 10, 10, WHITE, fnt20x12, (char*)name);

    display->hw_interface.set_delay(1000);
}
