#ifndef INC_FONTS_H_
#define INC_FONTS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#if 0
typedef struct {
    const uint16_t width;
    const uint16_t height;
    const uint16_t *data;
} tImage;
extern tImage image_cover;
#endif

typedef enum {
        fnt13x8=0,
        fnt20x12
}FontSize;

typedef struct{
    FontSize font_size;
    uint8_t col_number;
    uint8_t row_number;
    union {
        const void *raw;
        const uint8_t (*fnt13x8)[13];
        const uint16_t (*fnt20x12)[20];
    } font_data;
}FontDef;

//extern const uint8_t font5x8[95][5];
extern const uint8_t font13x8[224][13];
extern const uint16_t font20x12[254][20];

#ifdef __cplusplus
}
#endif

#endif /* INC_FONTS_H_ */
