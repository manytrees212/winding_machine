#ifndef DISP_ST7789_PORT_H
#define DISP_ST7789_PORT_H

#include <st7789.h>
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void hal_spi_transmit(const uint8_t*, uint16_t len);
void hal_set_cs_pin(bool state);
void hal_set_dc_pin(bool state);
void hal_set_rst_pin();
void hal_set_blk_pin(uint8_t brightness_percent);
void hal_set_delay(uint16_t ms);

#ifdef __cplusplus
}
#endif

#endif //DISP_ST7789_PORT_H