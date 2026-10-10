#include "ds18b20_array.h"
#include <string.h>
#include <math.h>

static bool empty(const uint8_t *rom)
{
    static const uint8_t zero[8] = {0};
    return memcmp(rom, zero, 8) == 0;
}

static size_t find(const DS18B20_Rom *roms, size_t count, const uint8_t *rom)
{
    for (size_t i = 0; i < count; ++i)
        if (memcmp(roms[i], rom, 8) == 0) return i;
    return count;
}

DS18B20_Status DS18B20_ReadTemperature(const OneWire_Bus *bus,
    const uint8_t rom[8], float *temperature)
{
    uint8_t pad[9];
    if (temperature == NULL || !DS18B20_IsValidRom(rom))
        return DS18B20_STATUS_INVALID_ARGUMENT;
    DS18B20_Status result = DS18B20_ReadScratchpad(bus, rom, pad);
    if (result != DS18B20_STATUS_OK) return result;
    float value = DS18B20_RawToCelsius(DS18B20_DecodeRaw(pad));
    if (value < -55.0F || value > 125.0F) return DS18B20_STATUS_OUT_OF_RANGE;
    *temperature = value;
    return DS18B20_STATUS_OK;
}

bool DS18B20_ArrayInit(DS18B20_Array *a, const OneWire_Bus *bus,
    DS18B20_Rom *roms, DS18B20_Rom *scan_buffer, float *temperatures,
    DS18B20_Status *status, size_t count)
{
    if (!a || !OneWire_IsValid(bus) || !roms || !scan_buffer ||
        roms == scan_buffer || !temperatures || !status || !count) return false;
    for (size_t i = 0; i < count; ++i) {
        if (!empty(roms[i]) && (!DS18B20_IsValidRom(roms[i]) ||
            find((const DS18B20_Rom *)roms, i, roms[i]) != i)) return false;
    }
    memset(a, 0, sizeof(*a));
    a->bus = bus; a->roms = roms; a->found = scan_buffer;
    a->temperatures = temperatures; a->status = status; a->count = count;
    a->phase = 1; /* Begin discovery even if the tick is near wraparound. */
    a->sample_interval_ms = 250U;
    OneWire_SearchReset(&a->search);
    for (size_t i = 0; i < count; ++i) {
        temperatures[i] = NAN;
        status[i] = DS18B20_STATUS_NO_DEVICE;
    }
    return true;
}

bool DS18B20_ArrayReplace(DS18B20_Array *a, size_t slot, const uint8_t rom[8])
{
    if (!a || a->phase != 0 || slot >= a->count || !DS18B20_IsValidRom(rom)) return false;
    size_t existing = find((const DS18B20_Rom *)a->roms, a->count, rom);
    if (existing != a->count && existing != slot) return false;
    memcpy(a->roms[slot], rom, 8);
    a->temperatures[slot] = NAN;
    a->status[slot] = DS18B20_STATUS_NO_DEVICE;
    return true;
}

static void reconcile(DS18B20_Array *a)
{
    size_t missing = 0, unknown = 0, missing_slot = 0, new_device = 0;
    for (size_t i = 0; i < a->count; ++i)
        if (!empty(a->roms[i]) && find((const DS18B20_Rom *)a->found, a->found_count, a->roms[i]) == a->found_count) {
            ++missing; missing_slot = i;
        }
    for (size_t i = 0; i < a->found_count; ++i)
        if (find((const DS18B20_Rom *)a->roms, a->count, a->found[i]) == a->count) {
            ++unknown; new_device = i;
        }
    if (missing == 1 && unknown == 1) {
        memcpy(a->roms[missing_slot], a->found[new_device], 8);
        a->temperatures[missing_slot] = NAN;
        a->status[missing_slot] = DS18B20_STATUS_NO_DEVICE;
    }
    /* Do not consume new ROMs in free slots while a replacement is ambiguous. */
    if (missing > 0 && !(missing == 1 && unknown == 1)) return;
    for (size_t i = 0; i < a->found_count; ++i) {
        if (find((const DS18B20_Rom *)a->roms, a->count, a->found[i]) != a->count) continue;
        for (size_t j = 0; j < a->count; ++j)
            if (empty(a->roms[j])) { memcpy(a->roms[j], a->found[i], 8); break; }
    }
}

bool DS18B20_ArrayProcess(DS18B20_Array *a, uint32_t now)
{
    if (!a || !a->bus) return false;
    switch (a->phase) {
    case 0:
        if ((int32_t)(now - a->next_sample) < 0) break;
        if ((int32_t)(now - a->next_scan) >= 0) {
            OneWire_SearchReset(&a->search);
            a->found_count = 0; a->overflow = false; a->scan_error = false;
            a->phase = 1;
        } else a->phase = 2;
        break;
    case 1: {
        uint8_t rom[8];
        if (OneWire_SearchNext(a->bus, &a->search, rom)) {
            if (DS18B20_IsValidRom(rom)) {
                if (a->found_count == a->count) {
                    a->overflow = true;
                    a->phase = 2;
                } else memcpy(a->found[a->found_count++], rom, 8);
            }
        } else {
            /* SearchNext resets state on CRC/protocol failure. Only a complete
             * enumeration can justify automatic replacement. */
            a->scan_error = !a->search.last_device;
            a->phase = 2;
        }
        if (a->phase == 2) {
            if (!a->overflow && !a->scan_error) reconcile(a);
            a->next_scan = now + 2000U;
        }
        break;
    }
    case 2: {
        DS18B20_Status result = DS18B20_StartConversion(a->bus, NULL);
        for (size_t i = 0; i < a->count; ++i) {
            a->temperatures[i] = NAN;
            a->status[i] = result == DS18B20_STATUS_OK ? DS18B20_STATUS_BUSY : result;
        }
        a->phase = result == DS18B20_STATUS_OK ? 3 : 5;
        break;
    }
    case 3:
        /* Timestamp on the next call, after the Convert command has returned.
         * Interrupts during command transmission cannot shorten the wait. */
        a->deadline = now + 760U;
        a->phase = 6;
        break;
    case 6:
        if ((int32_t)(now - a->deadline) < 0) break;
        if (!DS18B20_IsConversionReady(a->bus)) {
            for (size_t i = 0; i < a->count; ++i) a->status[i] = DS18B20_STATUS_TIMEOUT;
            a->phase = 5; break;
        }
        a->index = 0; a->phase = 4;
        break;
    case 4: {
        size_t i = a->index++;
        a->status[i] = empty(a->roms[i]) ? DS18B20_STATUS_NO_DEVICE :
            DS18B20_ReadTemperature(a->bus, a->roms[i], &a->temperatures[i]);
        if (a->index == a->count) a->phase = 5;
        break;
    }
    case 5:
        a->next_sample = now + a->sample_interval_ms;
        a->phase = 0;
        return true;
    default: a->phase = 0; break;
    }
    return false;
}
