#include "ds18b20_array.h"
#include <string.h>
#include <math.h>
#define CHECK(x) do { if (!(x)) return __LINE__; } while (0)

static DS18B20_Rom devices[4];
static size_t device_count, search_index;
static bool presence = true, ready = true, crc_bad, scan_bad, pad_zero;
static uint8_t pad[9];
static size_t byte_index;
bool OneWire_IsValid(const OneWire_Bus *b) { return b != 0; }
bool OneWire_Reset(const OneWire_Bus *b) { (void)b; byte_index = 0; return presence; }
void OneWire_WriteByte(const OneWire_Bus *b, uint8_t value) { (void)b; (void)value; }
uint8_t OneWire_ReadByte(const OneWire_Bus *b)
{
    (void)b;
    uint8_t value = pad_zero ? 0 : pad[byte_index];
    if (byte_index++ == 8 && crc_bad) value ^= 1;
    return value;
}
bool OneWire_ReadBit(const OneWire_Bus *b) { (void)b; return ready; }
uint8_t OneWire_Crc8(const uint8_t *d, size_t n)
{
    uint8_t crc=0;
    while(n--) { uint8_t value=*d++; for(int i=0;i<8;++i) {
        uint8_t mix=(crc^value)&1; crc>>=1; if(mix) crc^=0x8C; value>>=1;
    }}
    return crc;
}
void OneWire_SearchReset(OneWire_SearchState *s) { memset(s,0,sizeof(*s)); search_index=0; }
bool OneWire_SearchNext(const OneWire_Bus *b, OneWire_SearchState *s, uint8_t rom[8])
{
    (void)b;
    if(scan_bad) { OneWire_SearchReset(s); return false; }
    if(search_index == device_count) return false;
    memcpy(rom,devices[search_index++],8);
    s->last_device = search_index == device_count;
    return true;
}
static void make_rom(uint8_t *r,uint8_t serial)
{
    memset(r,0,8); r[0]=0x28; r[1]=serial; r[7]=OneWire_Crc8(r,7);
}
static void make_pad(int16_t raw,uint8_t config)
{
    pad[0]=(uint8_t)raw; pad[1]=(uint8_t)((uint16_t)raw>>8);
    pad[2]=75; pad[3]=70; pad[4]=config; pad[5]=0xFF; pad[6]=12; pad[7]=0x10;
    pad[8]=OneWire_Crc8(pad,8);
}
static bool cycle(DS18B20_Array *a,uint32_t start)
{
    for(uint32_t i=0;i<2000;++i) if(DS18B20_ArrayProcess(a,start+i)) return true;
    return false;
}
int main(void)
{
    DS18B20_Array a;
    OneWire_Bus bus={0};
    DS18B20_Rom roms[2]={{0}}, scan[2];
    float values[2]; DS18B20_Status status[2];
    make_rom(devices[0],1); make_rom(devices[1],2); make_rom(devices[2],3);
    make_pad(-162,0x7F);
    CHECK(DS18B20_IsValidRom(devices[0]));
    CHECK(!DS18B20_IsValidRom(roms[0]));
    CHECK(!DS18B20_ArrayInit(&a,&bus,roms,scan,values,status,0));
    CHECK(DS18B20_ArrayInit(&a,&bus,roms,scan,values,status,2));
    device_count=2;
    CHECK(cycle(&a,0));
    CHECK(memcmp(roms[0],devices[0],8)==0 && memcmp(roms[1],devices[1],8)==0);
    CHECK(values[0]==-10.125F && status[1]==DS18B20_STATUS_OK);
    CHECK(!DS18B20_ArrayReplace(&a,0,roms[1]));
    /* Remove first sensor, keep second and introduce a new ROM. */
    memcpy(devices[0],devices[2],8);
    CHECK(cycle(&a,3000));
    CHECK(memcmp(roms[0],devices[2],8)==0 && memcmp(roms[1],devices[1],8)==0);
    /* Two replacements are ambiguous: keep both original mappings. */
    make_rom(devices[0],4); make_rom(devices[1],5);
    CHECK(cycle(&a,6000));
    CHECK(roms[0][1]==3 && roms[1][1]==2);
    /* An incomplete search must never replace a mapping. */
    scan_bad=true; CHECK(cycle(&a,9000)); CHECK(roms[0][1]==3 && a.scan_error);
    scan_bad=false; device_count=3;
    CHECK(cycle(&a,12000)); CHECK(a.overflow && roms[0][1]==3);
    crc_bad=true; CHECK(cycle(&a,15000));
    CHECK(status[0]==DS18B20_STATUS_CRC_ERROR && isnan(values[0])); crc_bad=false;
    pad_zero=true; CHECK(cycle(&a,18000)); CHECK(status[0]==DS18B20_STATUS_CRC_ERROR); pad_zero=false;
    presence=false; CHECK(cycle(&a,21000)); CHECK(status[0]==DS18B20_STATUS_NO_DEVICE); presence=true;
    ready=false; CHECK(cycle(&a,24000)); CHECK(status[0]==DS18B20_STATUS_TIMEOUT); ready=true;
    make_pad(-161,0x1F); CHECK(DS18B20_DecodeRaw(pad)==-168);
    make_pad(2001,0x7F); float out=42;
    CHECK(DS18B20_ReadTemperature(&bus,roms[0],&out)==DS18B20_STATUS_OUT_OF_RANGE && out==42);
    make_pad(1360,0x7F); CHECK(DS18B20_ReadTemperature(&bus,roms[0],&out)==DS18B20_STATUS_OK && out==85);
    make_pad(400,0x7F);
    CHECK(DS18B20_ArrayInit(&a,&bus,roms,scan,values,status,2));
    device_count=2;
    CHECK(cycle(&a,0xFFFFFF00U)); CHECK(status[0]==DS18B20_STATUS_OK);
    return 0;
}
