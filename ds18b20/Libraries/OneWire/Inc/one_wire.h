#ifndef ONE_WIRE_H
#define ONE_WIRE_H
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif
/* release_line means high impedance (open drain), never driven high.
 * delay_us must be accurate. Critical callbacks must be paired and restore
 * the saved interrupt state. Provide them when interrupts affect bit timing. */
typedef struct {
    void *context;
    void (*drive_low)(void *context);
    void (*release_line)(void *context);
    bool (*read_line)(void *context);
    void (*delay_us)(void *context, uint32_t microseconds);
    uint32_t (*enter_critical)(void *context);
    void (*exit_critical)(void *context, uint32_t state);
} OneWire_Bus;
bool OneWire_IsValid(const OneWire_Bus *bus);
bool OneWire_Reset(const OneWire_Bus *bus);
void OneWire_WriteByte(const OneWire_Bus *bus, uint8_t byte);
bool OneWire_ReadBit(const OneWire_Bus *bus);
uint8_t OneWire_ReadByte(const OneWire_Bus *bus);
uint8_t OneWire_Crc8(const uint8_t *bytes, size_t length);
#ifdef __cplusplus
}
#endif
#endif
