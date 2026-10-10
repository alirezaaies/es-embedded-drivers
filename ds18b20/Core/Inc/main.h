#ifndef MAIN_H
#define MAIN_H
#ifdef __cplusplus
extern "C" {
#endif
#include "stm32f1xx_hal.h"
void Error_Handler(void);
/* USER CODE BEGIN Private defines */
/* USER CODE END Private defines */
#define ONE_WIRE_1_Pin GPIO_PIN_10
#define ONE_WIRE_1_GPIO_Port GPIOB
#ifdef __cplusplus
}
#endif
#endif
