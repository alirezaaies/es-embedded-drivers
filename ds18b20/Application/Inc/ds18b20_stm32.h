#ifndef DS18B20_STM32_H
#define DS18B20_STM32_H
#include "ds18b20.h"
/* Call once AFTER HAL, SystemClock_Config and MX_GPIO_Init. */
void DS18B20_STM32_Init(void);
/* One externally powered DS18B20 on the generated ONE_WIRE_1 pin.
 * Blocking foreground call; output changes only on success. */
DS18B20_Status DS18B20_STM32_ReadTemperature(float *celsius);
#endif
