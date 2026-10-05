#ifndef BUTTON_CONTROLLER_HPP
#define BUTTON_CONTROLLER_HPP

#include <cstdint>

#define HIGH true
#define LOW  false

#define START_STOP  1
#define SPEED_DOWN  2
#define SPEED_UP    3
#define SYNC        4
#define FWD_REV     5
#define FUNC        6

class ButtonController{
public:
    ButtonController()=default;
    ~ButtonController()=default;

    void ScanButtons();
    void ResetPushedButton();
    std::uint8_t GetPushedButton();
private:
    void SetPushedButton(const uint8_t button);
private:
    static constexpr std::uint16_t SCAN_UPDATE_INTERVAL_MS = 50;
    static constexpr std::uint8_t ROWS = 2;
    static constexpr std::uint8_t COLS = 3;
    static constexpr std::uint8_t button_matrix_[ROWS][COLS]={
        {START_STOP, SPEED_DOWN, SPEED_UP},
        {SYNC, FWD_REV, FUNC}
    };
    static constexpr std::uint8_t DEBOUNCE_THRESHOLD = 5;
    std::uint32_t next_update_time_=0;
    std::uint8_t build_in_btn_counter_ =0;
    std::uint8_t pushed_counter_[ROWS][COLS] = {{0,0,0},{0,0,0}};
    bool last_stable_state_[ROWS][COLS] = {{LOW, LOW, LOW},{LOW, LOW, LOW}};
    volatile std::uint8_t pushed_button_ = 0;


};

#endif //BUTTON_CONTROLLER_HPP