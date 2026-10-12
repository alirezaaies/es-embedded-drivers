#ifndef APP_H
#define APP_H
#include "ds18b20.h"
/* Latest COMPLETED attempt: the old value stays visible during conversion.
 * Always check valid/status; busy is separate. Counters wrap as uint32_t. */
extern volatile float g_temperature_c;
extern volatile bool g_temperature_valid;
extern volatile bool g_measurement_busy;
extern volatile DS18B20_Status g_temperature_status;
extern volatile uint32_t g_sample_cycles;
extern volatile uint32_t g_successful_samples;
extern volatile uint32_t g_last_success_ms;
void App_Init(void);
void App_Process(void);
#endif
