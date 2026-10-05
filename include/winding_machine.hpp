#ifndef WINDING_MACHINE_HPP
#define WINDING_MACHINE_HPP

#include <cstdint>
#include "motor.hpp"
#include <st7789.h>
#include "display_st7789.h"
#include "flash_data_controller.hpp"
#include "motor.hpp"
#include "encoder.hpp"
#include "pid.hpp"
#include "button_controller.hpp"

enum class Function {Off, On};
enum class Sync {Off, On};

class WindingMachine{
public:
    WindingMachine()=delete;
    WindingMachine(ST7789_display* display,
                   FlashDataController* flash_ctrl,
                   MotorDriver<BTS7960Config>* motor_driver,
                   MotorDriver<DRV8871Config>* feeder_driver,
                   EncoderController* encoder_ctrl,
                   PID* pid,
                   ButtonController* button_ctrl);

    ~WindingMachine()=default;

public:
    void ProcessButtons(uint8_t button);
    void UpdateMotorFlashData();
    void UpdateFeederFlashData();
    void CorrectFeederPos();

    void ResetTrueFeederPosFlag();
    void SetTrueFeederPosFlag();
    bool GetTrueFeederPosFlag()const;

    void ResetHomeFlag();
    void SetHomeFlag();
    bool GetHomeFlag()const;

    std::uint16_t CalcFeederPosition(std::int32_t counts);
    std::uint16_t GetTurnPosition()const;

    void StopMachine();

    void DisplayCurrentTurns()const;
    void DisplayTotalTurns()const;
    void DisplayCurrentSpeed()const;
    void DisplayWireDiameter()const;
    void DisplayMotorDirection()const;
    void DisplayFeederDirection()const;
    void DisplayMotorState()const;
    void DisplayFeederState()const;
    void DisplayFuncButton()const;
    void DisplayBobbinRPM()const;
    void DisplaySyncState()const;
    void DisplayFeederPosition()const;
    void DisplayMaxFeederPos()const;
    void DisplayKp()const;
    void DisplayKi()const;
    void DisplayKd()const;

    std::uint8_t GetDisplayBobbinRPM()const;
    std::uint8_t GetDisplaySpeed()const;
    std::uint16_t GetDisplayTurns()const;
    std::uint16_t GetDisplayFeederPos()const;

private:
    void DisplayMainPage()const;

    void ProcessButtonOne();
    void ProcessButtonTwo();
    void ProcessButtonThree();
    void ProcessButtonFour();
    void ProcessButtonFive();
    void ProcessButtonSix();
    void ProcessButtonSeven();

    void SynchronizeTheFeeder();

    void FeederFree_StartStop();
    void FeederFree_SpeedUp();
    void FeederFree_SpeedDown();

    void FeederSlave_StartStop();
#if 0
    //void FeederSlave_SpeedUp();
    //void FeederSlave_SpeedDown();
#endif
private:
    static constexpr std::uint8_t MOTOR_SPEED_STEP{21};
    static constexpr std::uint8_t FEEDER_POS_UPDATE_INTERVAL_MS{10};
private:
    ST7789_display* const display_;
    FlashDataController* const flash_data_ctrl_;
    MotorDriver<BTS7960Config>* const motor_driver_;
    MotorDriver<DRV8871Config>* const feeder_driver_;
    EncoderController* const encoder_ctrl_;
    PID* const pid_ctrl_;
    ButtonController* const button_ctrl_;

    Function func_button_ = Function::Off;
    Sync sync_button_ = Sync::Off;

    std::uint32_t last_feeder_pos_correction_time_{0};

    std::uint16_t display_turns_{0};
    std::uint8_t display_bobbin_rpm_{0};
    std::uint8_t display_speed_{0};
    std::uint8_t display_feeder_pos_{0};

    std::uint8_t motor_log_cnt_{0};
    std::uint8_t feeder_log_cnt_{0};
    bool true_feeder_pos_flag_{true};
    bool home_pos_flag_{false};
    bool sync_flag_{false};
};

#endif //WINDING_MACHINE_HPP