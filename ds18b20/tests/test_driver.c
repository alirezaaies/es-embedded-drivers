#include "ds18b20.h"
#include <string.h>
#define CHECK(x) do { if (!(x)) return __LINE__; } while (0)
static uint8_t scratch[9], sent[4];
static unsigned reset_count, read_count, sent_count, waits, ready_after;
static unsigned missing_reset;
static void action(void *c) { (void)c; }
static bool line(void *c) { (void)c; return true; }
static void us(void *c, uint32_t n) { (void)c; (void)n; }
static void ms(void *c, uint32_t n) { (void)c; waits += n; }
bool OneWire_IsValid(const OneWire_Bus *b) { return b && b->drive_low; }
bool OneWire_Reset(const OneWire_Bus *b)
{ (void)b; return ++reset_count != missing_reset; }
void OneWire_WriteByte(const OneWire_Bus *b, uint8_t n)
{ (void)b; if (sent_count < 4) sent[sent_count++] = n; }
bool OneWire_ReadBit(const OneWire_Bus *b)
{ (void)b; return waits >= ready_after; }
uint8_t OneWire_ReadByte(const OneWire_Bus *b)
{ (void)b; return scratch[read_count++]; }
/* Independent host-bus CRC fixture; actual CRC implementation is tested
 * separately against published scratchpad vectors. */
uint8_t OneWire_Crc8(const uint8_t *d, size_t n)
{
    unsigned crc = 0;
    for (size_t i = 0; i < n; ++i) {
        crc ^= d[i];
        for (unsigned j = 0; j < 8; ++j)
            crc = (crc & 1) ? (crc >> 1) ^ 0x8c : crc >> 1;
    }
    return (uint8_t)crc;
}
static void fixture(int raw, uint8_t config)
{
    uint8_t bytes[9] = {(uint8_t)raw, (uint8_t)((uint16_t)raw >> 8),
                        75, 70, config, 0xff, 0x0c, 0x10, 0};
    memcpy(scratch, bytes, 9);
    scratch[8] = OneWire_Crc8(scratch, 8);
    reset_count = read_count = sent_count = waits = ready_after = missing_reset = 0;
}
int main(void)
{
    DS18B20 sensor = {{0,action,action,line,us,0,0},ms};
    float t = -999;
    CHECK(DS18B20_ReadTemperature(0,&t) == DS18B20_INVALID_ARGUMENT);
    CHECK(DS18B20_ReadTemperature(&sensor,0) == DS18B20_INVALID_ARGUMENT);
    sensor.delay_ms = 0;
    CHECK(DS18B20_ReadTemperature(&sensor,&t) == DS18B20_INVALID_ARGUMENT);
    sensor.delay_ms = ms;
    fixture(401,0x7f); ready_after = 750;
    CHECK(DS18B20_ReadTemperature(&sensor,&t) == DS18B20_OK && t == 25.0625f);
    CHECK(waits == 750 && read_count == 9 && reset_count == 2 && sent_count == 4);
    CHECK(sent[0] == 0xcc && sent[1] == 0x44 && sent[2] == 0xcc && sent[3] == 0xbe);
    fixture(-162,0x7f);
    CHECK(DS18B20_ReadTemperature(&sensor,&t) == DS18B20_OK && t == -10.125f);
    for (unsigned resolution = 0; resolution < 4; ++resolution) {
        fixture(407,(uint8_t)(0x1f | resolution << 5));
        CHECK(DS18B20_ReadTemperature(&sensor,&t) == DS18B20_OK);
        CHECK(t == (float)(407 & ~((1 << (3-resolution))-1)) / 16.0f);
        fixture(-161,(uint8_t)(0x1f | resolution << 5));
        CHECK(DS18B20_ReadTemperature(&sensor,&t) == DS18B20_OK);
        CHECK(t == (float)(-161 & ~((1 << (3-resolution))-1)) / 16.0f);
    }
    fixture(-880,0x7f); CHECK(DS18B20_ReadTemperature(&sensor,&t) == DS18B20_OK && t == -55);
    fixture(2000,0x7f); CHECK(DS18B20_ReadTemperature(&sensor,&t) == DS18B20_OK && t == 125);
    fixture(1360,0x7f); CHECK(DS18B20_ReadTemperature(&sensor,&t) == DS18B20_OK && t == 85);
    t = -999;
    fixture(2001,0x7f); CHECK(DS18B20_ReadTemperature(&sensor,&t) == DS18B20_INVALID_DATA && t == -999);
    fixture(-881,0x7f); CHECK(DS18B20_ReadTemperature(&sensor,&t) == DS18B20_INVALID_DATA && t == -999);
    fixture(400,0x7f); missing_reset = 1;
    CHECK(DS18B20_ReadTemperature(&sensor,&t) == DS18B20_NO_DEVICE && sent_count == 0 && t == -999);
    fixture(400,0x7f); missing_reset = 2;
    CHECK(DS18B20_ReadTemperature(&sensor,&t) == DS18B20_NO_DEVICE && t == -999);
    fixture(400,0x7f); ready_after = 801;
    CHECK(DS18B20_ReadTemperature(&sensor,&t) == DS18B20_TIMEOUT && waits == 800 && t == -999);
    fixture(400,0x7f); ready_after = 800;
    CHECK(DS18B20_ReadTemperature(&sensor,&t) == DS18B20_OK);
    t = -999;
    fixture(400,0x7f); scratch[0] ^= 1;
    CHECK(DS18B20_ReadTemperature(&sensor,&t) == DS18B20_CRC_ERROR && t == -999);
    fixture(0,0x1f); memset(scratch,0,9);
    CHECK(DS18B20_ReadTemperature(&sensor,&t) == DS18B20_INVALID_DATA && t == -999);
    for (unsigned i = 4; i <= 7; ++i) {
        fixture(400,0x7f); scratch[i] ^= 1; scratch[8] = OneWire_Crc8(scratch,8);
        CHECK(DS18B20_ReadTemperature(&sensor,&t) == DS18B20_INVALID_DATA && t == -999);
    }
    return 0;
}
