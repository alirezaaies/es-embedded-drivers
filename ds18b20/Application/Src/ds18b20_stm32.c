#include "ds18b20_stm32.h"
#include "main.h"
static DS18B20 sensor;
static void low(void *context)
{
    (void)context;
    ONE_WIRE_1_GPIO_Port->BRR = ONE_WIRE_1_Pin;
}
static void release(void *context)
{
    (void)context;
    ONE_WIRE_1_GPIO_Port->BSRR = ONE_WIRE_1_Pin;
}
static bool read_line(void *context)
{
    (void)context;
    return (ONE_WIRE_1_GPIO_Port->IDR & ONE_WIRE_1_Pin) != 0;
}
static void delay_us(void *context, uint32_t us)
{
    (void)context;
    uint32_t start = DWT->CYCCNT;
    uint32_t cycles = us * (SystemCoreClock / 1000000U);
    while ((uint32_t)(DWT->CYCCNT - start) < cycles) { }
}
static void delay_ms(void *context, uint32_t ms)
{
    (void)context;
    HAL_Delay(ms);
}
static uint32_t lock(void *context)
{
    (void)context;
    uint32_t state = __get_PRIMASK();
    __disable_irq();
    return state;
}
static void unlock(void *context, uint32_t state)
{
    (void)context;
    __set_PRIMASK(state);
}
void DS18B20_STM32_Init(void)
{
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CYCCNT = 0;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
    const DS18B20 binding = {
        {NULL, low, release, read_line, delay_us, lock, unlock}, delay_ms
    };
    sensor = binding;
    release(NULL);
}

DS18B20_Status DS18B20_STM32_ReadTemperature(float *celsius)
{
    return DS18B20_ReadTemperature(&sensor, celsius);
}
