// button_controller_wrapper.h
#ifndef BUTTON_CONTROLLER_WRAPPER_H
#define BUTTON_CONTROLLER_WRAPPER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

// C-compatible interface for button controller
void ButtonController_Scan();
uint8_t ButtonController_GetButton();

#ifdef __cplusplus
}
#endif

#endif // BUTTON_CONTROLLER_WRAPPER_H