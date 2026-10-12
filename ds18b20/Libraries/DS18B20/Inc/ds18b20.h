#ifndef DS18B20_H
#define DS18B20_H
#include "one_wire.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef enum {
    DS18B20_OK = 0,
    DS18B20_INVALID_ARGUMENT,
    DS18B20_NO_DEVICE,
    DS18B20_TIMEOUT,
    DS18B20_CRC_ERROR,
    DS18B20_INVALID_DATA
} DS18B20_Status;
typedef struct {
    OneWire_Bus bus;
    /* Wait at least this many milliseconds; interrupts must be enabled. */
    void (*delay_ms)(void *context, uint32_t milliseconds);
} DS18B20;
/* Exactly ONE externally powered DS18B20 on the bus. Blocking (about 860 ms
 * maximum with accurate delays, excluding interrupt overhead). Call from main,
 * never ISR. Serialize access to the bus. Output changes ONLY on success.
 * No ROM search, configuration writes, EEPROM, or parasite power support. */
DS18B20_Status DS18B20_ReadTemperature(const DS18B20 *sensor, float *celsius);
#ifdef __cplusplus
}
#endif
#endif
