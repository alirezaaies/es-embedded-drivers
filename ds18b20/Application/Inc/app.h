#ifndef APP_H
#define APP_H
#include "ds18b20_array.h"
#ifndef APP_SENSOR_COUNT
#define APP_SENSOR_COUNT 4U
#endif
extern DS18B20_Rom g_sensor_roms[APP_SENSOR_COUNT];
extern float g_temperature_c[APP_SENSOR_COUNT];
extern DS18B20_Status g_temperature_status[APP_SENSOR_COUNT];
extern volatile uint32_t g_sample_cycles;
void App_Init(void);
void App_Process(void);
#endif
