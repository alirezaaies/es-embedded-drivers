#include "app.h"
#include "main.h"

DS18B20_Rom g_sensor_roms[APP_SENSOR_COUNT];
float g_temperature_c[APP_SENSOR_COUNT];
DS18B20_Status g_temperature_status[APP_SENSOR_COUNT];
volatile uint32_t g_sample_cycles;
static DS18B20_Rom found[APP_SENSOR_COUNT];
static DS18B20_Array sensors;
static OneWire_Bus bus;

static void low(void *ctx) { (void)ctx; ONE_WIRE_1_GPIO_Port->BRR = ONE_WIRE_1_Pin; }
static void release(void *ctx) { (void)ctx; ONE_WIRE_1_GPIO_Port->BSRR = ONE_WIRE_1_Pin; }
static bool read_line(void *ctx) { (void)ctx; return (ONE_WIRE_1_GPIO_Port->IDR & ONE_WIRE_1_Pin) != 0; }
static void delay_us(void *ctx, uint32_t us)
{
    (void)ctx;
    uint32_t start = DWT->CYCCNT;
    uint32_t cycles = us * (SystemCoreClock / 1000000U);
    while ((uint32_t)(DWT->CYCCNT - start) < cycles) { }
}
static uint32_t lock(void *ctx) { (void)ctx; uint32_t saved = __get_PRIMASK(); __disable_irq(); return saved; }
static void unlock(void *ctx, uint32_t saved) { (void)ctx; __set_PRIMASK(saved); }

void App_Init(void)
{
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CYCCNT = 0;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
    const OneWire_Bus binding = {NULL, low, release, read_line, delay_us, lock, unlock};
    if (!OneWire_Init(&bus, &binding) ||
        !DS18B20_ArrayInit(&sensors, &bus, g_sensor_roms, found,
            g_temperature_c, g_temperature_status, APP_SENSOR_COUNT)) Error_Handler();
}
void App_Process(void)
{
    if (DS18B20_ArrayProcess(&sensors, HAL_GetTick())) ++g_sample_cycles;
}
