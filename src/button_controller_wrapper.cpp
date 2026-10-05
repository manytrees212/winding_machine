#include "button_controller_wrapper.h"
#include "button_controller.hpp"

// global pointer (defined in main.cpp)
extern ButtonController* button_controller_ptr;

void ButtonController_Scan() {
    if(button_controller_ptr){
        button_controller_ptr->ScanButtons();
    }
}

uint8_t ButtonController_GetButton() {
    if(button_controller_ptr){
        return button_controller_ptr->GetPushedButton();
    }
    return 0;
}