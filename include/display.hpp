#ifndef DISPLAY_HPP
#define DISPLAY_HPP

#include <cstdint>
#include "st7789.h"

class Display{
public:
    explicit Display(ST7789_display* disp_st7789)
    :disp_st7789_(disp_st7789){
    }
    ~Display();
public:
    void ShowMainPage();
    void UpdateTotalTurns(uint16_t turns_number);
    void UpdateCurrentTurns(uint16_t turns_number);
    void UpdateCurrentSpeed(uint8_t speed);
    void UpdateFuncButton(bool is_on);
private:
    const ST7789_display* disp_st7789_;
    uint16_t current_turns_=0;
    uint16_t total_turns_=0;
    uint8_t current_speed_=0;
    uint8_t func_button_=0;
};

#endif //DISPLAY_HPP