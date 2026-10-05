#ifndef GPIO_H
#define GPIO_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <stm32f1xx_hal.h>

/*================================*/
/*   defines for build-in LED     */
/*================================*/
#define BUILD_IN_LED_PB2 GPIO_PIN_2

/*================================*/
/*   defines for Display_st7789   */
/*================================*/
#define RST_GPIO_Port   GPIOA
#define RST_Pin         GPIO_PIN_3
#define DC_GPIO_Port    GPIOA
#define DC_Pin          GPIO_PIN_2
#define BLK_GPIO_Port   GPIOA
#define BLK_Pin         GPIO_PIN_4

/*================================*/
/*   defines for button pins      */
/*================================*/
#define ROW1_PIN        GPIO_PIN_12
#define ROW2_PIN        GPIO_PIN_15
#define COL1_PIN        GPIO_PIN_3
#define COL2_PIN        GPIO_PIN_8
#define COL3_PIN        GPIO_PIN_9

/*================================*/
/*   defines for motor pins       */
/*================================*/
#define L_EN_PIN        GPIO_PIN_10
#define R_EN_PIN        GPIO_PIN_11
#define LPWM_PIN        GPIO_PIN_7
#define RPWM_PIN        GPIO_PIN_6
#define L_IS_PIN        GPIO_PIN_0
#define R_IS_PIN        GPIO_PIN_1

/*================================*/
/*   defines for feeder pins      */
/*================================*/
#define IN1_PIN         GPIO_PIN_13
#define IN2_PIN         GPIO_PIN_14

/*================================*/
/*defines for sensor home position*/
/*================================*/
#define HOME_POS_PIN    GPIO_PIN_12

/*================================*/
/*defines for rpm motor encoder   */
/*================================*/
#define MOTOR_ENCODER_CHANNEL_A     GPIO_PIN_0
#define MOTOR_ENCODER_CHANNEL_B     GPIO_PIN_1

/*================================*/
/*defines for rpm feeder encoderb */
/*================================*/
#define FEEDER_ENCODER_CHANNEL_A     GPIO_PIN_4
#define FEEDER_ENCODER_CHANNEL_B     GPIO_PIN_5

void GPIO_Init(void);

void GPIO_ToggleLED(void);

void LED_On(void);

void LED_Off(void);

#ifdef __cplusplus
}
#endif

#endif // GPIO_H
