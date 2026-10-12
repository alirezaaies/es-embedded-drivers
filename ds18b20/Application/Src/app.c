#include "app.h"
#include "ds18b20_stm32.h"
#include "main.h"

volatile float g_temperature_c;
volatile bool g_temperature_valid;
volatile bool g_measurement_busy;
volatile DS18B20_Status g_temperature_status = DS18B20_NO_DEVICE;
volatile uint32_t g_sample_cycles;
volatile uint32_t g_successful_samples;
volatile uint32_t g_last_success_ms;

void App_Init(void)
{
    DS18B20_STM32_Init();
}

void App_Process(void)
{
    float value;
    g_measurement_busy = true;
    DS18B20_Status status = DS18B20_STM32_ReadTemperature(&value);
    /* Publish the latest completed attempt. Keep its validity visible while
     * the next conversion is running; busy reports that conversion separately. */
    g_temperature_status = status;
    g_temperature_valid = (status == DS18B20_OK);
    if (g_temperature_valid) {
        g_temperature_c = value;
        ++g_successful_samples;
        g_last_success_ms = HAL_GetTick();
    }
    ++g_sample_cycles;
    g_measurement_busy = false;
    HAL_Delay(250U);
}
