#include "ds18b20.h"
DS18B20_Status DS18B20_ReadTemperature(const DS18B20 *sensor, float *celsius)
{
    uint8_t scratchpad[9];
    uint16_t bits;
    int32_t raw;
    uint32_t elapsed = 0;
    const OneWire_Bus *bus;
    if (!sensor || !celsius || !sensor->delay_ms ||
        !OneWire_IsValid(&sensor->bus)) return DS18B20_INVALID_ARGUMENT;
    bus = &sensor->bus;
    if (!OneWire_Reset(bus)) return DS18B20_NO_DEVICE;
    OneWire_WriteByte(bus, 0xCC); /* Skip ROM: one sensor only. */
    OneWire_WriteByte(bus, 0x44); /* Convert T. */
    while (!OneWire_ReadBit(bus)) {
        if (elapsed >= 800U) return DS18B20_TIMEOUT;
        sensor->delay_ms(bus->context, 1U);
        ++elapsed;
    }
    if (!OneWire_Reset(bus)) return DS18B20_NO_DEVICE;
    OneWire_WriteByte(bus, 0xCC);
    OneWire_WriteByte(bus, 0xBE); /* Read scratchpad. */
    for (uint8_t i = 0; i < 9U; ++i) scratchpad[i] = OneWire_ReadByte(bus);
    if (OneWire_Crc8(scratchpad, 8U) != scratchpad[8]) return DS18B20_CRC_ERROR;
    /* Nine zero bytes pass CRC: verify fixed bytes and configuration too. */
    if ((scratchpad[4] & 0x9FU) != 0x1FU || scratchpad[5] != 0xFFU ||
        scratchpad[6] != 0x0CU || scratchpad[7] != 0x10U)
        return DS18B20_INVALID_DATA;
    bits = (uint16_t)scratchpad[0] | ((uint16_t)scratchpad[1] << 8U);
    /* Ignore undefined low bits at 9/10/11-bit resolution. */
    bits &= (uint16_t)(0xFFFFU << (3U - ((scratchpad[4] >> 5U) & 3U)));
    raw = (bits & 0x8000U) ? (int32_t)bits - 65536 : (int32_t)bits;
    if (raw < -55 * 16 || raw > 125 * 16) return DS18B20_INVALID_DATA;
    *celsius = (float)raw / 16.0f;
    return DS18B20_OK;
}
