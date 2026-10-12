#include "one_wire.h"
static uint32_t lock(const OneWire_Bus *bus)
{
    return bus->enter_critical ? bus->enter_critical(bus->context) : 0U;
}
static void unlock(const OneWire_Bus *bus, uint32_t state)
{
    if (bus->exit_critical) bus->exit_critical(bus->context, state);
}
bool OneWire_IsValid(const OneWire_Bus *bus)
{
    return bus && bus->drive_low && bus->release_line && bus->read_line &&
           bus->delay_us && ((bus->enter_critical == NULL) ==
                            (bus->exit_critical == NULL));
}
bool OneWire_Reset(const OneWire_Bus *bus)
{
    uint32_t state;
    bool presence;
    if (!OneWire_IsValid(bus)) return false;
    bus->release_line(bus->context);
    bus->delay_us(bus->context, 5U);
    if (!bus->read_line(bus->context)) return false;
    /* Keep reset low within its intended width even under interrupt load. */
    state = lock(bus);
    bus->drive_low(bus->context);
    bus->delay_us(bus->context, 480U);
    bus->release_line(bus->context);
    bus->delay_us(bus->context, 70U);
    presence = !bus->read_line(bus->context);
    unlock(bus, state);
    bus->delay_us(bus->context, 410U);
    return presence && bus->read_line(bus->context);
}
static void write_bit(const OneWire_Bus *bus, bool value)
{
    uint32_t state = lock(bus);
    bus->drive_low(bus->context);
    bus->delay_us(bus->context, value ? 6U : 60U);
    bus->release_line(bus->context);
    unlock(bus, state);
    bus->delay_us(bus->context, value ? 64U : 10U);
}
void OneWire_WriteByte(const OneWire_Bus *bus, uint8_t byte)
{
    if (!OneWire_IsValid(bus)) return;
    for (uint8_t i = 0; i < 8U; ++i) {
        write_bit(bus, (byte & 1U) != 0U);
        byte >>= 1U;
    }
}
bool OneWire_ReadBit(const OneWire_Bus *bus)
{
    uint32_t state;
    bool value;
    if (!OneWire_IsValid(bus)) return false;
    state = lock(bus);
    bus->drive_low(bus->context);
    bus->delay_us(bus->context, 3U);
    bus->release_line(bus->context);
    bus->delay_us(bus->context, 10U);
    value = bus->read_line(bus->context);
    unlock(bus, state);
    bus->delay_us(bus->context, 53U);
    return value;
}
uint8_t OneWire_ReadByte(const OneWire_Bus *bus)
{
    uint8_t byte = 0;
    for (uint8_t i = 0; i < 8U; ++i)
        if (OneWire_ReadBit(bus)) byte |= (uint8_t)(1U << i);
    return byte;
}
uint8_t OneWire_Crc8(const uint8_t *bytes, size_t length)
{
    uint8_t crc = 0;
    if (!bytes) return 0;
    while (length--) {
        uint8_t byte = *bytes++;
        for (uint8_t i = 0; i < 8U; ++i) {
            bool mix = ((crc ^ byte) & 1U) != 0U;
            crc >>= 1U;
            if (mix) crc ^= 0x8CU;
            byte >>= 1U;
        }
    }
    return crc;
}
