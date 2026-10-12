#include "app.h"
#include "ds18b20_stm32.h"
#define CHECK(x) do { if (!(x)) return __LINE__; } while (0)
static DS18B20_Status next_status;
static float next_value;
static uint32_t tick;
static bool previous_valid, observed_busy, bad_valid, bad_delay, initialized;
void DS18B20_STM32_Init(void) { initialized = true; }
DS18B20_Status DS18B20_STM32_ReadTemperature(float *celsius)
{
    observed_busy = g_measurement_busy;
    if (g_temperature_valid != previous_valid) bad_valid = true;
    if (next_status == DS18B20_OK) *celsius = next_value;
    return next_status;
}
uint32_t HAL_GetTick(void) { return tick; }
void HAL_Delay(uint32_t delay)
{
    if (delay != 250U || g_measurement_busy) bad_delay = true;
}
int main(void)
{
    CHECK(!g_temperature_valid && !g_measurement_busy);
    CHECK(g_temperature_status == DS18B20_NO_DEVICE);
    CHECK(g_sample_cycles == 0 && g_successful_samples == 0);
    App_Init(); CHECK(initialized);
    next_status = DS18B20_OK; next_value = 23.5f; tick = 1000;
    previous_valid = false;
    App_Process();
    CHECK(observed_busy && !bad_valid && !bad_delay && !g_measurement_busy);
    CHECK(g_temperature_valid && g_temperature_c == 23.5f);
    CHECK(g_sample_cycles == 1 && g_successful_samples == 1 && g_last_success_ms == 1000);
    next_status = DS18B20_OK; next_value = 24.0f; tick = 2000;
    previous_valid = true; App_Process();
    CHECK(!bad_valid && g_temperature_valid && g_temperature_c == 24.0f);
    CHECK(g_sample_cycles == 2 && g_successful_samples == 2 && g_last_success_ms == 2000);
    next_status = DS18B20_CRC_ERROR; tick = 3000;
    App_Process();
    CHECK(!bad_valid && !g_temperature_valid && g_temperature_c == 24.0f);
    CHECK(g_temperature_status == DS18B20_CRC_ERROR);
    CHECK(g_sample_cycles == 3 && g_successful_samples == 2 && g_last_success_ms == 2000);
    next_status = DS18B20_OK; next_value = 24.5f; tick = 4000;
    previous_valid = false; App_Process();
    CHECK(!bad_valid && !bad_delay && !g_measurement_busy && g_temperature_valid);
    CHECK(g_sample_cycles == 4 && g_successful_samples == 3 && g_last_success_ms == 4000);
    return 0;
}
