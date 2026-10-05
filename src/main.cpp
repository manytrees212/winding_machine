#include <cstdint>

#include <eeprom.h>
#include <logger.h>
#include <logger_with_hal.h>
#include <error_handler.h>

#include "main.hpp"
#include "project_config.hpp"
#include "gpio.h"
#include "tim.h"
#include "exti.h"
#include "chip_init.hpp"
#include "eeprom_data_init.hpp"

#include "flash_data_controller.hpp"
#include "button_controller.hpp"
#include "display_st7789.h"
#include "motor.hpp"
#include "encoder.hpp"
#include "pid.hpp"
#include "uart_cmd_parser.hpp"
#include "winding_machine.hpp"

extern "C" const uint16_t VirtAddVarTab[NB_OF_VAR] = {/*defenition for eeprom*/
    VAR_CURRENT_TURNS,
    VAR_TOTAL_TURNS,
    VAR_FEEDER_COUNTS,
    VAR_MAX_FEEDER_COUNTS,
    VAR_WIRE_DIAMETER_SCALED,
    VAR_KP_SCALED,
    VAR_KI_SCALED,
    VAR_KD_SCALED,
    VAR_CAN_READ,
    VAR_NEED_WRITE
};

/*global pointer for Callback function*/
WindingMachine* machine_ptr = nullptr;

extern "C" void Feeder_Home_Position_Callback(){

    HAL_NVIC_DisableIRQ(EXTI15_10_IRQn);
    if(!machine_ptr){
        return;
    }
    machine_ptr->StopMachine();
    machine_ptr->SetHomeFlag();
}

int main(){

    Chip_Init();

    static ProjectConfig config;
    EEPROM_Data_Init(&VirtAddVarTab[0], config);

    static FlashDataController flash_data_controller(&config, &VirtAddVarTab[0]);
    if (config.need_write == 1) {
        flash_data_controller.WriteFlashData();
    }
    LOG_INFO("Configuration EEPROM:\
                current_turns=%d,\
                total_turns=%d,\
                feeder_counts=%d,\
                max_feeder_counts=%d",
                flash_data_controller.GetFlashCurrentTurns(),
                flash_data_controller.GetFlashTotalTurns(),
                flash_data_controller.GetFlashFeederCounts(),
                flash_data_controller.GetMaxFlashFeederCounts()
             );

    ST7789_HW_Interface hw = {/*functions for display init*/
        .spi_transmit = hal_spi_transmit,
        .set_cs_pin = hal_set_cs_pin,
        .set_dc_pin = hal_set_dc_pin,
        .set_rst_pin = hal_set_rst_pin,
        .set_blk_pin = hal_set_blk_pin,
        .set_delay = hal_set_delay
    };

    static ST7789_display disp_st7789;
    if(ST7789_init(&disp_st7789, &hw)!=Disp_st7789_OK){
        LOG_ERROR("Display ST7789 Init() FAILED");
    }else{
        LOG_INFO("Display ST7789 READY");
    }

    static ButtonController button_controller;

    static MotorDriver<BTS7960Config> motor;
    static MotorDriver<DRV8871Config> feeder;

    static EncoderController encoder_ctrl(&htim2,
                                          &htim3,
                                          flash_data_controller.GetFlashCurrentTurns());

    static PID pid(flash_data_controller.GetKpScaled(),
                   flash_data_controller.GetKiScaled(),
                   flash_data_controller.GetKdScaled(),
                   0,
                   100);

    static UartCmdParser uart_cmd_parser;

    static WindingMachine winding_machine(&disp_st7789,
                                   &flash_data_controller,
                                   &motor,
                                   &feeder,
                                   &encoder_ctrl,
                                   &pid,
                                   &button_controller);
    machine_ptr=&winding_machine;

    /*to prevent extra display updating*/
    auto last_bobbin_rpm = winding_machine.GetDisplayBobbinRPM();
    auto last_speed = winding_machine.GetDisplaySpeed();
    auto last_turn = winding_machine.GetDisplayTurns();
    auto last_feeder_pos = winding_machine.CalcFeederPosition(encoder_ctrl.GetFeederCounts());

    std::uint32_t last_ms = HAL_GetTick();

    while(1){
        //process home position
        if(winding_machine.GetHomeFlag()){

            winding_machine.ResetHomeFlag();

            if(!winding_machine.GetTrueFeederPosFlag()){
                winding_machine.SetTrueFeederPosFlag();
                encoder_ctrl.StartFeederCounting();
            }

            encoder_ctrl.ResetFeederCounts();
            flash_data_controller.SetFlashFeederCounts(0);

            auto current_feeder_counts=encoder_ctrl.GetFeederCounts();
            LOG_INFO("FEEDER is HOME:\
                     encoder_counts=%d\
                     flash_counts_data=%d\
                     feeder_pos=%d",
                     current_feeder_counts,
                     flash_data_controller.GetFlashFeederCounts(),
                     winding_machine.CalcFeederPosition(current_feeder_counts));

            winding_machine.DisplayFeederPosition();

            feeder.SetDirection(Direction::Forward);
            winding_machine.DisplayFeederDirection();
        }

        //scan and process buttons
        std::uint8_t current_button = 0;
        button_controller.ScanButtons(); //internal filter: update period 50 ms
        current_button = button_controller.GetPushedButton();
        if(current_button){
            winding_machine.ProcessButtons(current_button);
        }

        //block loop while feeder position wrong
        if(!winding_machine.GetTrueFeederPosFlag()){
            continue;
        }

        auto is_motor = motor.GetState()==MotorState::Running;
        auto is_feeder = feeder.GetState()==MotorState::Running;

        if(is_motor){
            winding_machine.UpdateMotorFlashData();
        }

        if(is_feeder){
            winding_machine.UpdateFeederFlashData();
            LED_On();
        }

        if(is_motor && is_feeder){
            winding_machine.CorrectFeederPos();
        }

        if(!is_motor && !is_feeder){
            uart_cmd_parser.ParseUartBuffer();
            if(uart_cmd_parser.IsCmdReady()){
                flash_data_controller.ProcessUartCommand(uart_cmd_parser.GetUartCmd());
                if(flash_data_controller.GetPidCoefUpdFlag()){

                    flash_data_controller.ResetPidCoefUpdFlag();

                    pid.SetGains(flash_data_controller.GetKpScaled(),
                                 flash_data_controller.GetKiScaled(),
                                 flash_data_controller.GetKdScaled());
                }
                winding_machine.DisplayTotalTurns();
                winding_machine.DisplayFeederPosition();
                winding_machine.DisplayWireDiameter();
                winding_machine.DisplayKp();
                winding_machine.DisplayKi();
                winding_machine.DisplayKd();
            }
        }

        auto now_ms=HAL_GetTick();
        if((now_ms-last_ms) > 1000){/*update display 1000 ms*/
            if(is_motor){
                auto bobbin_rpm = winding_machine.GetDisplayBobbinRPM();
                auto turns = winding_machine.GetDisplayTurns();
                auto speed = winding_machine.GetDisplaySpeed();
                if(last_bobbin_rpm != bobbin_rpm){
                    winding_machine.DisplayBobbinRPM();
                    last_bobbin_rpm=bobbin_rpm;
                }
                if(turns != last_turn){
                    winding_machine.DisplayCurrentTurns();
                    last_turn=turns;
                }
                if(speed != last_speed){
                    winding_machine.DisplayCurrentSpeed();
                    last_speed=speed;
                }
            }
            if(is_feeder){
                auto feeder_pos = winding_machine.GetDisplayFeederPos();
                if(feeder_pos != last_feeder_pos){
                    winding_machine.DisplayFeederPosition();
                    last_feeder_pos=feeder_pos;
                }
            }
            last_ms=now_ms;
        }
    }
}
