#ifndef DS18B20_ARRAY_H
#define DS18B20_ARRAY_H
#include "ds18b20.h"
#ifdef __cplusplus
extern "C" {
#endif

/* Capacity is chosen by the caller: no heap, no global state. ROMs are mutable
 * so an unambiguous replacement can retain its index. Zero ROM = empty slot.
 * All buffers and the bus must outlive the array. Call only from one task.
 */
typedef uint8_t DS18B20_Rom[8];
typedef struct {
    const OneWire_Bus *bus;
    DS18B20_Rom *roms, *found;
    float *temperatures;
    DS18B20_Status *status;
    size_t count, found_count, index;
    uint32_t deadline, next_scan, next_sample;
    uint32_t sample_interval_ms; /* Idle gap after a cycle; default 250 ms. */
    uint8_t phase;
    bool overflow, scan_error;
    OneWire_SearchState search;
} DS18B20_Array;

bool DS18B20_ArrayInit(DS18B20_Array *array, const OneWire_Bus *bus,
    DS18B20_Rom *roms, DS18B20_Rom *scan_buffer, float *temperatures,
    DS18B20_Status *status, size_t count);
/* Call frequently with a monotonic millisecond tick. Returns true after a
 * complete sampling cycle. Conversion wait is nonblocking; each call performs
 * at most one ROM search (~15 ms) or one addressed read (~12 ms).
 */
bool DS18B20_ArrayProcess(DS18B20_Array *array, uint32_t now_ms);
/* Read a previously converted sensor, without starting a new conversion. */
DS18B20_Status DS18B20_ReadTemperature(const OneWire_Bus *bus,
    const uint8_t rom[8], float *temperature);
/* Explicit replacement for ambiguous changes; rejects duplicate/invalid ROM. */
bool DS18B20_ArrayReplace(DS18B20_Array *array, size_t slot, const uint8_t rom[8]);
#ifdef __cplusplus
}
#endif
#endif
