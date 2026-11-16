#include "winding_machine.hpp"

#include <logger.h>
#include <logger_with_hal.h>

#include <cstdio>
#include "gpio.h"

WindingMachine::WindingMachine(ST7789_display* display,
                               FlashDataController* flash_ctrl,
                               MotorDriver<BTS7960ConfigMotor>* motor_driver,
                               MotorDriver<BTS7960ConfigFeeder>* feeder_driver,
                               EncoderController* encoder_ctrl,
                               PID* pid,
                               ButtonController* button_ctrl)
    : display_(display)
    , flash_data_ctrl_(flash_ctrl)
    , motor_driver_(motor_driver)
    , feeder_driver_(feeder_driver)
    , encoder_ctrl_(encoder_ctrl)
    , pid_ctrl_(pid)
    , button_ctrl_(button_ctrl){
}

bool WindingMachine::WindingMachineInit(){

    if(!display_){
        LOG_ERROR("display_ is invalid");
        return false;
    }
    if(!flash_data_ctrl_){
        LOG_ERROR("flash_data_ctrl_ is invalid");
        return false;
    }
    if(!motor_driver_){
        LOG_ERROR("motor_driver_ is invalid");
        return false;
    }
    if(!feeder_driver_){
        LOG_ERROR("feeder_driver_ is invalid");
        return false;
    }
    if(!encoder_ctrl_){
        LOG_ERROR("encoder_ctrl_ is invalid");
        return false;
    }
    if(!pid_ctrl_){
        LOG_ERROR("pid_ctrl_ is invalid");
        return false;
    }
    if(!button_ctrl_){
        LOG_ERROR("button_ctrl_ is invalid");
        return false;
    }

    auto flash_feeder_counts = flash_data_ctrl_->GetFlashFeederCounts();
    if(flash_feeder_counts==0){
        if(HAL_GPIO_ReadPin(GPIOB, HOME_POS_PIN)!=GPIO_PIN_RESET){
            LOG_INFO("WM init: WRONG FEEDER POS");
            ResetTrueFeederPosFlag();
            ResetHomeFlag();
            encoder_ctrl_->StopFeederCounting();
        }
    }else{
        if(HAL_GPIO_ReadPin(GPIOB, HOME_POS_PIN)==GPIO_PIN_RESET){
            LOG_INFO("WM init: RESET FEEDER COUNT");
            flash_data_ctrl_->SetFlashFeederCounts(0);
            flash_feeder_counts=0;
        }
    }

    if(true_feeder_pos_flag_){
        encoder_ctrl_->ResetFeederCounts();/*to clean timer*/
        encoder_ctrl_->SetFeederCounts(flash_feeder_counts);

        auto feeder_pos = CalcFeederPositionScaled(static_cast<std::int32_t>(flash_feeder_counts));
        auto scale = encoder_ctrl_->GetScaleFactorTurns();
        display_feeder_pos_=static_cast<std::uint8_t>(feeder_pos/scale);

        LOG_INFO("WM init: turn_pos=%d, feeder_pos=%d",
                 CalcTurnPositionScaled(),
                 CalcFeederPositionScaled(encoder_ctrl_->GetFeederCounts())
                 );
    }

    auto flash_current_turns_scaled = flash_data_ctrl_->GetFlashCurrentTurnsScaled();
    display_turns_=static_cast<std::uint16_t>(flash_current_turns_scaled/flash_data_ctrl_->GetScale());

    base_layer_turns_scaled_=flash_data_ctrl_->GetFlashBaseLayerTurnsScaled();
    display_turn_pos_=CalcTurnPositionScaled()/encoder_ctrl_->GetScaleFactorTurns();

    DisplayMainPage();
    LOG_INFO("WM init: true_feeder_flag=%d, home_flag=%d, sync_flag=%d",
             true_feeder_pos_flag_,
             home_pos_flag_,
             sync_flag_);

    return true;
}

void WindingMachine::DisplayMainPage()const{
    ST7789_set_screen_orientation(display_, landscape);
    ST7789_clear_screen(display_, WHITE);

    ST7789_draw_string(display_, 5, 10, BLACK,  fnt20x12, "turns:");
    DisplayCurrentTurns();

    ST7789_draw_string(display_, 5, 30, BLACK,  fnt20x12, "total:");
    DisplayTotalTurns();

    ST7789_draw_string(display_, 5, 50, BLACK,  fnt20x12, "speed:");
    DisplayCurrentSpeed();

    ST7789_draw_string(display_, 160, 10, BLACK,  fnt20x12, "Kp:");
    DisplayKp();

    ST7789_draw_string(display_, 160, 30, BLACK,  fnt20x12, "Ki:");
    DisplayKi();

    ST7789_draw_string(display_, 160, 50, BLACK,  fnt20x12, "Kd:");
    DisplayKd();

    ST7789_draw_string(display_, 160, 70, BLACK,  fnt20x12, "\xF8:");
    DisplayWireDiameter();

    ST7789_draw_string(display_, 5, 90, BLACK,  fnt20x12, "motor:");
    DisplayMotorDirection();
    DisplayMotorState();
    DisplayBobbinRPM();

    ST7789_draw_string(display_, 5, 110, BLACK, fnt20x12, "feeder:");
    DisplayFeederDirection();
    DisplayFeederState();
    DisplayFeederPosition();

    ST7789_draw_string(display_, 5, 190, BLACK, fnt20x12, "sync:");
    DisplaySyncState();

    ST7789_draw_string(display_, 5, 170, BLACK, fnt20x12, "fn:");
    DisplayFuncButton();

    ST7789_draw_string(display_, 5, 130, BLACK, fnt20x12, "base_pos:");
    DisplayTurnPos();
}

void WindingMachine::DisplayCurrentTurns()const{

    PixelCoord left_top = {89, 10};
    PixelCoord right_bottom = {155, 25};
    ST7789_draw_rectangle(display_, left_top, right_bottom, WHITE);

    char buffer[6];
    snprintf(buffer, sizeof(buffer), "%u", display_turns_);
    ST7789_draw_string(display_, 90, 10, BLACK, fnt20x12, buffer);
}

void WindingMachine::DisplayTotalTurns()const{

    PixelCoord left_top = {89, 30};
    PixelCoord right_bottom = {155, 45};
    ST7789_draw_rectangle(display_, left_top, right_bottom, WHITE);

    char buffer[6];
    snprintf(buffer, sizeof(buffer), "%u", flash_data_ctrl_->GetFlashTotalTurns());
    ST7789_draw_string(display_, 90, 30, BLACK, fnt20x12, buffer);
}

void WindingMachine::DisplayCurrentSpeed()const{

    PixelCoord left_top = {89, 50};
    PixelCoord right_bottom = {155, 65};
    ST7789_draw_rectangle(display_, left_top, right_bottom, WHITE);

    char buffer[6];
    snprintf(buffer, sizeof(buffer), "%u", display_speed_);
    ST7789_draw_string(display_, 90, 50, BLACK, fnt20x12, buffer);
}

void WindingMachine::DisplayKp()const{
    PixelCoord left_top = {199, 10};
    PixelCoord right_bottom = {239, 25};
    ST7789_draw_rectangle(display_, left_top, right_bottom, WHITE);

    auto Kp = flash_data_ctrl_->GetKpScaled();
    char buffer[6];
    snprintf(buffer, sizeof(buffer), "%u", Kp);
    ST7789_draw_string(display_, 200, 10, BLACK, fnt20x12, buffer);
}

void WindingMachine::DisplayKi()const{
    PixelCoord left_top = {199, 30};
    PixelCoord right_bottom = {239, 45};
    ST7789_draw_rectangle(display_, left_top, right_bottom, WHITE);

    auto Ki = flash_data_ctrl_->GetKiScaled();
    char buffer[6];
    snprintf(buffer, sizeof(buffer), "%u", Ki);
    ST7789_draw_string(display_, 200, 30, BLACK, fnt20x12, buffer);
}

void WindingMachine::DisplayKd()const{
    PixelCoord left_top = {199, 50};
    PixelCoord right_bottom = {239, 65};
    ST7789_draw_rectangle(display_, left_top, right_bottom, WHITE);

    auto Kd = flash_data_ctrl_->GetKdScaled();
    char buffer[6];
    snprintf(buffer, sizeof(buffer), "%u", Kd);
    ST7789_draw_string(display_, 200, 50, BLACK, fnt20x12, buffer);
}

void WindingMachine::DisplayWireDiameter()const{
    PixelCoord left_top = {199, 70};
    PixelCoord right_bottom = {239, 85};
    ST7789_draw_rectangle(display_, left_top, right_bottom, WHITE);

    char buffer[6];
    auto wire_diam = flash_data_ctrl_->GetScaledFlashWireDiam();
    snprintf(buffer, sizeof(buffer), "%u", wire_diam);
    ST7789_draw_string(display_, 200, 70, BLACK, fnt20x12, buffer);
}

void WindingMachine::DisplayMotorDirection()const{

    PixelCoord left_top = {109, 90};
    PixelCoord right_bottom = {150, 105};
    ST7789_draw_rectangle(display_, left_top, right_bottom, WHITE);

    auto direction = motor_driver_->GetDirection();

    switch(direction){

        case Direction::Forward:
            ST7789_draw_string(display_, 110, 90, BLACK, fnt20x12, "FWD");
            break;

        case Direction::Reverse:
            ST7789_draw_string(display_, 110, 90, BLACK, fnt20x12, "REV");
            break;

    default: LOG_ERROR("Unknown motor direction");
    }
}

void WindingMachine::DisplayFeederDirection()const{

    PixelCoord left_top = {109, 110};
    PixelCoord right_bottom = {150, 125};
    ST7789_draw_rectangle(display_, left_top, right_bottom, WHITE);

    auto direction = feeder_driver_->GetDirection();

    switch(direction){

    case Direction::Forward:
            ST7789_draw_string(display_, 110, 110, BLACK, fnt20x12, "FWD");
            break;

    case Direction::Reverse:
            ST7789_draw_string(display_, 110, 110, BLACK, fnt20x12, "REV");
            break;
    }

}

void WindingMachine::DisplayMotorState()const{

    PixelCoord rec_start = {160, 90};
    PixelCoord rec_end = {180,105};

    auto state = motor_driver_->GetState();
    switch(state){        
    case MotorState::Idle:
            ST7789_draw_rectangle(display_, rec_start, rec_end, RED);
            break;
    case MotorState::Running:
            ST7789_draw_rectangle(display_, rec_start, rec_end, GREEN);
            break;
    }
}

void WindingMachine::DisplayFeederState()const{

    PixelCoord rec_start = {160, 110};
    PixelCoord rec_end = {180,125};

    auto state = feeder_driver_->GetState();
    switch(state){
    case MotorState::Idle:
            ST7789_draw_rectangle(display_, rec_start, rec_end, RED);
            break;
        case MotorState::Running:
            ST7789_draw_rectangle(display_, rec_start, rec_end, GREEN);
            break;
    }
}

void WindingMachine::DisplaySyncState()const{
    PixelCoord left_top = {79, 189};
    PixelCoord right_bottom = {120, 205};

    ST7789_draw_rectangle(display_, left_top, right_bottom, WHITE);

    const char* value = static_cast<bool>(sync_button_) ? "ON" : "OFF";
    ST7789_draw_string(display_, 80, 190, BLACK, fnt20x12, value);
}

void WindingMachine::DisplayFuncButton()const{

    PixelCoord left_top = {50, 169};
    PixelCoord right_bottom = {100, 185};
    ST7789_draw_rectangle(display_, left_top, right_bottom, WHITE);

    const char* value = static_cast<bool>(func_button_) ? "ON" : "OFF";
    ST7789_draw_string(display_, 50, 170, BLACK, fnt20x12, value);
}

void WindingMachine::DisplayTurnPos()const{
    PixelCoord left_top = {189, 130};
    PixelCoord right_bottom = {239, 145};
    ST7789_draw_rectangle(display_, left_top, right_bottom, WHITE);

    char buffer[6];
    snprintf(buffer, sizeof(buffer), "%u", display_turn_pos_);
    ST7789_draw_string(display_, 190, 130, BLACK, fnt20x12, buffer);
}

void WindingMachine::DisplayBobbinRPM()const{
    PixelCoord left_top = {189, 90};
    PixelCoord right_bottom = {239, 105};
    ST7789_draw_rectangle(display_, left_top, right_bottom, WHITE);

    char buffer[6];
    if(motor_driver_->GetState()==MotorState::Idle){
        snprintf(buffer, sizeof(buffer), "%u", 0);
    }else{
        snprintf(buffer, sizeof(buffer), "%u", display_bobbin_rpm_);
    }
    ST7789_draw_string(display_, 190, 90, PURPLE, fnt20x12, buffer);
}

void WindingMachine::DisplayFeederPosition()const{
    PixelCoord left_top = {189, 110};
    PixelCoord right_bottom = {239, 125};
    ST7789_draw_rectangle(display_, left_top, right_bottom, WHITE);

    char buffer[6];
    snprintf(buffer, sizeof(buffer), "%u", display_feeder_pos_);
    ST7789_draw_string(display_, 190, 110, PURPLE, fnt20x12, buffer);
}

void WindingMachine::ProcessButtons(uint8_t button){
    switch(button){
    case 0: return;
    case 1: ProcessButtonOne();break;
    case 2: ProcessButtonTwo();break;
    case 3: ProcessButtonThree();break;
    case 4: ProcessButtonFour();break;
    case 5: ProcessButtonFive();break;
    case 6: ProcessButtonSix();break;
    default: LOG_ERROR("Unknown button");
    }
}

/*============*/
/* Button One */
/*============*/
void WindingMachine::ProcessButtonOne(){

    if(feeder_driver_->GetDirection()==Direction::Reverse){
        __HAL_GPIO_EXTI_CLEAR_IT(HOME_POS_PIN);
        HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
    }else{
        HAL_NVIC_DisableIRQ(EXTI15_10_IRQn);
    }

    switch(func_button_){
        case Function::Off:
            if(sync_button_==Sync::Off){
                StartStop();
            }
            else{
                StartStopSync();
            }
            break;

        case Function::On:
            if(!true_feeder_pos_flag_){
                LOG_ERROR("WRONG FEEDER POSITION!");
            }
            if(HAL_GPIO_ReadPin(GPIOB, HOME_POS_PIN)==GPIO_PIN_RESET){
                feeder_driver_->SetDirection(Direction::Forward);
                DisplayFeederDirection();
                encoder_ctrl_->ResetFeederCounts();
            }
            if(feeder_driver_->GetState()==MotorState::Idle){
                feeder_driver_->SetSpeed(feeder_driver_->GetMaxSpeed());
                FeederStart();
            }else{
                FeederStop();
            }
            break;
    }
    motor_log_cnt_=0;
    feeder_log_cnt_=10;
}

void WindingMachine::StartStop(){
    if(motor_driver_->GetState()==MotorState::Idle){
        MotorStart();
    }else{
        MotorStop();
    }
}

void WindingMachine::StartStopSync(){

    if(motor_driver_->GetState()==MotorState::Idle){
        motor_driver_->SetDirection(Direction::Forward);
        DisplayMotorDirection();
        MotorFeederStart();
    }else{
        MotorFeederStop();
        last_feeder_pos_correction_time_=0;
    }
}

/*============*/
/* Button Two */
/*============*/
void WindingMachine::ProcessButtonTwo(){

    switch(func_button_){

        case Function::Off:{
            SpeedDown();
            return;
        }
        case Function::On: return;
    }
}

void WindingMachine::SpeedDown(){

    auto current_speed = motor_driver_->GetSpeed();
    if(current_speed==0){
        return;
    }

    auto min_speed = motor_driver_->GetMinSpeed();
    if(current_speed > min_speed){
        current_speed-=MOTOR_SPEED_STEP;
        motor_driver_->SetSpeed(current_speed);
        display_speed_=current_speed;
    }else{
        motor_driver_->SetSpeed(0);
        display_speed_=0;
    }
}

/*==============*/
/* Button Three */
/*==============*/
void WindingMachine::ProcessButtonThree(){

    switch(func_button_){
        case Function::Off:{
            SpeedUp();
            return;
        }
        case Function::On:{
            StopMachine();
            UpdateDisplayAfterStop();
            flash_data_ctrl_->ResetFlashData();
            return;
        }
    }
}

void WindingMachine::SpeedUp(){

    auto current_speed = motor_driver_->GetSpeed();
    auto min_speed = motor_driver_->GetMinSpeed();
    if(current_speed==0){
        current_speed=min_speed;
        motor_driver_->SetSpeed(current_speed);
        display_speed_=current_speed;
        return;
    }

    auto max_speed = motor_driver_->GetMaxSpeed();
    if(current_speed < max_speed){
        current_speed+=MOTOR_SPEED_STEP;
    }else{
        current_speed=max_speed;
    }

    motor_driver_->SetSpeed(current_speed);
    display_speed_=current_speed;
}

/*=============*/
/* Button Four */
/*=============*/
void WindingMachine::ProcessButtonFour(){

    StopMachine();
    UpdateDisplayAfterStop();

    switch(func_button_){
        case Function::Off:
            if(sync_button_==Sync::Off){

                (SynchronizeFeeder()) ? SyncOn() : SyncOff();

                if(sync_flag_){
                    auto motor_dir = motor_driver_->GetDirection();
                    if(motor_dir!=Direction::Forward){
                        motor_driver_->SetDirection(Direction::Forward);
                        DisplayMotorDirection();
                    }
                }
            }else{
                SyncOff();
            }
            break;
        case Function::On:
            auto current_feeder_counts = encoder_ctrl_->GetFeederCounts();
            auto flash_data = (current_feeder_counts<0) ? 0 : current_feeder_counts;
            flash_data_ctrl_->SetFlashFeederCounts(static_cast<uint16_t>(flash_data));

            encoder_ctrl_->UpdateTurnNumber();            
            flash_data_ctrl_->SetFlashCurrentTurnsScaled(encoder_ctrl_->GetTurnNumberScaled());

            UpdateBaseLayerTurns(encoder_ctrl_->GetDeltaCounts());
            flash_data_ctrl_->SetFlashBaseLayerTurnsScaled(static_cast<uint16_t>(base_layer_turns_scaled_));

            flash_data_ctrl_->WriteFlashData();
            break;
    }
}

void WindingMachine::MotorChangeDirection(){
    auto motor_dir = motor_driver_->GetDirection();
    (motor_dir==Direction::Forward) ?
        motor_driver_->SetDirection(Direction::Reverse) : motor_driver_->SetDirection(Direction::Forward);
    DisplayMotorDirection();
}

void WindingMachine::FeederChangeDirection(){

    auto feeder_dir = feeder_driver_->GetDirection();

    if(HAL_GPIO_ReadPin(GPIOB, HOME_POS_PIN)==GPIO_PIN_RESET && feeder_dir==Direction::Forward){
        return;
    }

    (feeder_dir==Direction::Forward) ?
        feeder_driver_->SetDirection(Direction::Reverse) : feeder_driver_->SetDirection(Direction::Forward);
    DisplayFeederDirection();
}

/*=============*/
/* Button Five */
/*=============*/
void WindingMachine::ProcessButtonFive(){

    StopMachine();
    UpdateDisplayAfterStop();

    switch(func_button_){        
        case Function::Off:{
            if(sync_button_==Sync::Off){
                MotorChangeDirection();
                break;
            }else{
                FeederChangeDirection();
                break;
            }
        }
        case Function::On:{
            FeederChangeDirection();
            break;
        }
    }
}

/*============*/
/* Button Six */
/*============*/
void WindingMachine::ProcessButtonSix(){

    StopMachine();
    UpdateDisplayAfterStop();

    (func_button_==Function::Off) ? func_button_=Function::On : func_button_=Function::Off;
    if(func_button_==Function::On){
        SyncOff();
    }
    DisplayFuncButton();
}

/*SUPPORT METHODS*/

void WindingMachine::UpdateBaseLayerTurns(std::int16_t delta){

    if(motor_driver_->GetDirection()==Direction::Reverse){

        auto abs_delta = (delta > 0) ? delta : -delta;
        base_layer_turns_scaled_-=abs_delta;

        if(base_layer_turns_scaled_<0){
            base_layer_turns_scaled_=0;
        }
        return;
    }

    if(feeder_driver_->GetDirection()==Direction::Forward){
        base_layer_turns_scaled_+=delta;
    }else{
        base_layer_turns_scaled_-=delta;
        if(base_layer_turns_scaled_<0){
            base_layer_turns_scaled_=0;
        }
    }
}

void WindingMachine::UpdateMotorFlashData(){

    encoder_ctrl_->UpdateMotorEncoderData();
    if(!encoder_ctrl_->MotorEncoderHasUpdated()){
        return;
    }

    UpdateBaseLayerTurns(encoder_ctrl_->GetDeltaCounts());
    display_turn_pos_ = CalcTurnPositionScaled()/encoder_ctrl_->GetScaleFactorTurns();

    auto bobbin_rpm = encoder_ctrl_->GetBobbinRPM();
    display_bobbin_rpm_=bobbin_rpm;

    auto current_turns = static_cast<std::uint32_t>(encoder_ctrl_->GetTurnNumber());
    auto total_turns = static_cast<std::uint32_t>(flash_data_ctrl_->GetFlashTotalTurns());
    if(current_turns > total_turns && motor_driver_->GetDirection()==Direction::Forward){
        StopMachine();
        UpdateDisplayAfterStop();
        LOG_INFO("Bobbin is FULL");
        return;
    }
    display_turns_=current_turns;
    flash_data_ctrl_->SetFlashCurrentTurnsScaled(current_turns);

    //for logger
    if(motor_log_cnt_==20){
        LOG_INFO("turns_pos=%d", CalcTurnPositionScaled());
        motor_log_cnt_=0;
    }
    ++motor_log_cnt_;
}

void WindingMachine::UpdateFeederFlashData(){

    encoder_ctrl_->UpdateFeederEncoderData();
    if(!encoder_ctrl_->FeederEncoderHasUpdated()){
        return;
    }
    auto current_feeder_counts = encoder_ctrl_->GetFeederCounts();
    auto flash_data = (current_feeder_counts<0) ? 0 : current_feeder_counts;
    flash_data_ctrl_->SetFlashFeederCounts(static_cast<uint16_t>(flash_data));

    auto feeder_pos = CalcFeederPositionScaled(current_feeder_counts);
    display_feeder_pos_=static_cast<std::uint8_t>(feeder_pos/encoder_ctrl_->GetScaleFactorTurns());

    if(feeder_log_cnt_==20){
        LOG_INFO("feeder_pos=%d", feeder_pos);
        feeder_log_cnt_=0;
    }
    ++feeder_log_cnt_;
}

void WindingMachine::UpdateDisplayAfterStop(){

    if(display_update_flag_){
        return;
    }

    DisplayMotorState();
    DisplayFeederState();

    DisplayMotorDirection();
    DisplayFeederDirection();

    DisplayBobbinRPM();
    DisplayFeederPosition();

    UpdateMotorFlashData();
    UpdateFeederFlashData();

    display_update_flag_=true;
}

void WindingMachine::StopMachine(){
    motor_driver_->Stop();
    feeder_driver_->Stop();
}

std::uint16_t WindingMachine::CalcTurnPositionScaled(){

    encoder_ctrl_->UpdateTurnNumber();
    if(!encoder_ctrl_->MotorEncoderHasUpdated()){
        UpdateBaseLayerTurns(encoder_ctrl_->GetDeltaCounts());
    }
    auto wire_diam_scaled = flash_data_ctrl_->GetScaledFlashWireDiam();
    auto scale = flash_data_ctrl_->GetScale();
    auto turn_pos_scaled = static_cast<std::uint32_t>(base_layer_turns_scaled_)*wire_diam_scaled / scale;

    return static_cast<std::uint16_t>(turn_pos_scaled);
}

std::uint16_t WindingMachine::CalcFeederPositionScaled(std::int32_t counts){
    auto counts_per_revolution = encoder_ctrl_->GetCountsPerRevolution();
    auto scale = encoder_ctrl_->GetScaleFactorTurns(); //because current_turns_scaled
    auto feeder_pos = counts*scale/counts_per_revolution; //pitch 1 mm is missed
    if(feeder_pos < 0){
        feeder_pos = 0;
    }
    return static_cast<std::uint16_t>(feeder_pos);
}

bool WindingMachine::SynchronizeFeeder(){

    if(!true_feeder_pos_flag_){
        LOG_INFO("FEEDER POSITION WRONG");
        return false;
    }

    if(sync_flag_){
        LOG_INFO("SYNCHRONIZATION DONE");
        return true;
    }

    auto pos_in_layer = CalcTurnPositionScaled();
    auto feeder_pos = CalcFeederPositionScaled(encoder_ctrl_->GetFeederCounts());
    LOG_INFO("Sync_Start: TurnPos=%d, FeederPos=%d", pos_in_layer, feeder_pos);

    std::int32_t error = pos_in_layer - feeder_pos;

    const std::int32_t max_error = 16;
    auto abs_error = (error>0) ? error : -error;
    if(abs_error < max_error){
        LOG_INFO("SYNCHRONIZATION DONE, error=%d", abs_error);
        return true;
    }

    if(error>0){
        feeder_driver_->SetDirection(Direction::Forward);
        DisplayFeederDirection();
    }else{
        feeder_driver_->SetDirection(Direction::Reverse);
        DisplayFeederDirection();
        __HAL_GPIO_EXTI_CLEAR_IT(HOME_POS_PIN);
        HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
        error=-error;
    }

    feeder_driver_->SetSpeed(feeder_driver_->GetMaxSpeed());
    FeederStart();

    auto timeout = HAL_GetTick();
    std::int32_t error_last = abs_error;
    while(error > max_error){

        feeder_pos = CalcFeederPositionScaled(encoder_ctrl_->GetFeederCounts());
        error = pos_in_layer - feeder_pos;
        error = (error>0) ? error : -error;

        if(error <= error_last){
            error_last = error;
        }else{
            FeederStop();
            LOG_INFO("Sync STOPPED: error increased");
            return false;
        }

        button_ctrl_->ScanButtons();
        if(button_ctrl_->GetPushedButton()==START_STOP){
            FeederStop();
            LOG_INFO("Sync STOPPED: pushed button stop");
            return false;
        }

        if(HAL_GetTick()-timeout > 10000){
            FeederStop();
            LOG_INFO("SynchronizeFeeder TIMEOUT");
            SyncOff();
            return false;
        }
    }
    FeederStop();
    LOG_INFO("SYNCHRONIZATION DONE");
    LOG_INFO("Sync_Stop: TurnPos=%d, FeederPos=%d", pos_in_layer, feeder_pos);
    return true;
}

void WindingMachine::CorrectFeederPos(){

    if(last_feeder_pos_correction_time_==0){
        last_feeder_pos_correction_time_=HAL_GetTick();
        return;
    }

    auto dt_ms = static_cast<std::uint16_t>(HAL_GetTick() - last_feeder_pos_correction_time_);
    if (dt_ms < 50){
        return;
    }
    last_feeder_pos_correction_time_ = HAL_GetTick();

    auto turn_pos = CalcTurnPositionScaled();
    display_turn_pos_=turn_pos/encoder_ctrl_->GetScaleFactorTurns();

    auto feeder_pos = CalcFeederPositionScaled(encoder_ctrl_->GetFeederCounts());
    display_feeder_pos_=static_cast<std::uint8_t>(feeder_pos/encoder_ctrl_->GetScaleFactorTurns());

    std::uint16_t speed = 0;
    if(feeder_driver_->GetDirection()==Direction::Forward){
        speed = pid_ctrl_->ComputeSpeed(turn_pos, feeder_pos,  dt_ms);
    }else{
        speed = pid_ctrl_->ComputeSpeed(feeder_pos, turn_pos,  dt_ms);
    }

    auto feeder_pwm = static_cast<std::uint8_t>((speed*feeder_driver_->GetMaxSpeed())/100);
    feeder_driver_->SetSpeed(feeder_pwm);
}

std::uint8_t WindingMachine::GetDisplayBobbinRPM()const{
    return display_bobbin_rpm_;
}

std::uint8_t WindingMachine::GetDisplaySpeed()const{
    return display_speed_;
}

std::uint16_t WindingMachine::GetDisplayTurns()const{
    return display_turns_;
}

std::uint16_t WindingMachine::GetDisplayTurnPos()const{
    return display_turn_pos_;
}

std::uint16_t WindingMachine::GetDisplayFeederPos()const{
    return display_feeder_pos_;
}

bool WindingMachine::GetTrueFeederPosFlag()const{
    return true_feeder_pos_flag_;
}

void WindingMachine::SetTrueFeederPosFlag(){
     true_feeder_pos_flag_=true;
}

void WindingMachine::ResetTrueFeederPosFlag(){
    true_feeder_pos_flag_=false;
}

void WindingMachine::SetHomeFlag(){
    home_pos_flag_=true;
}

void WindingMachine::ResetHomeFlag(){
    home_pos_flag_=false;
}

bool WindingMachine::GetHomeFlag()const{
    return home_pos_flag_;
}

void WindingMachine::SyncOn(){
    sync_flag_=true;
    sync_button_ = Sync::On;
    DisplaySyncState();
    LED_On();
}

void WindingMachine::SyncOff(){
    sync_flag_=false;
    sync_button_ = Sync::Off;
    DisplaySyncState();
    LED_Off();
}

void WindingMachine::FeederStart(){    
    feeder_driver_->Start();

    DisplayFeederState();

    SyncOff();
    display_update_flag_=false;
}

void WindingMachine::FeederStop(){
    feeder_driver_->Stop();

    DisplayFeederState();
    DisplayFeederPosition();

    UpdateFeederFlashData();
}

void WindingMachine::MotorStart(){
    motor_driver_->Start();

    DisplayMotorState();

    SyncOff();
    display_update_flag_=false;
}

void WindingMachine::MotorStop(){
    motor_driver_->Stop();

    DisplayMotorState();
    DisplayBobbinRPM();

    UpdateMotorFlashData();    
}

void WindingMachine::MotorFeederStart(){

    if(!sync_flag_){
        return;
    }
    motor_driver_->Start();
    feeder_driver_->Start();

    DisplayMotorState();
    DisplayFeederState();

    display_update_flag_=false;
}

void WindingMachine::MotorFeederStop(){
    feeder_driver_->Stop();
    motor_driver_->Stop();

    DisplayMotorState();
    DisplayFeederState();

    DisplayFeederPosition();
    DisplayBobbinRPM();

    UpdateMotorFlashData();
    UpdateFeederFlashData();
}