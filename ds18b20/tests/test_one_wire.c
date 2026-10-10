#include "one_wire.h"
#include <string.h>
#define CHECK(x) do { if (!(x)) return __LINE__; } while (0)
static bool samples[256];
static size_t sample_index, length;
static uint32_t held, maximum;
static bool locked;
static void action(void *ctx) { (void)ctx; }
static bool read_line(void *ctx) { (void)ctx; return sample_index<length ? samples[sample_index++] : true; }
static void delay(void *ctx,uint32_t us) { (void)ctx; if(locked) held+=us; }
static uint32_t enter(void *ctx) { (void)ctx; locked=true; held=0; return 7; }
static void leave(void *ctx,uint32_t state) { (void)ctx; if(state!=7) maximum=9999; if(held>maximum) maximum=held; locked=false; }
static void reset_samples(bool presence)
{
    sample_index=0; length=3; samples[0]=true; samples[1]=!presence; samples[2]=true;
}
int main(void)
{
    OneWire_Bus bus, binding={0,action,action,read_line,delay,enter,leave};
    CHECK(OneWire_Init(&bus,&binding));
    binding.exit_critical=0; CHECK(!OneWire_Init(&bus,&binding));
    reset_samples(true); CHECK(OneWire_Reset(&bus)); CHECK(maximum==70 && !locked);
    reset_samples(false); CHECK(!OneWire_Reset(&bus));
    reset_samples(true); samples[0]=false; CHECK(!OneWire_Reset(&bus));
    maximum=0; OneWire_WriteBit(&bus,true); CHECK(maximum==6 && !locked);
    maximum=0; OneWire_WriteBit(&bus,false); CHECK(maximum==60 && !locked);
    maximum=0; sample_index=0; length=1; samples[0]=false;
    CHECK(!OneWire_ReadBit(&bus)); CHECK(maximum==13 && !locked);
    uint8_t scratch[8]={0x50,0x05,0x4B,0x46,0x7F,0xFF,0x0C,0x10};
    CHECK(OneWire_Crc8(scratch,8)==0x1C);
    uint8_t rom[8]={0x28,1,2,3,4,5,6,0}, found[8];
    rom[7]=OneWire_Crc8(rom,7);
    reset_samples(true);
    for(size_t bit=0;bit<64;++bit) {
        bool value=(rom[bit/8]>>(bit%8))&1;
        samples[length++]=value; samples[length++]=!value;
    }
    OneWire_SearchState search;
    CHECK(OneWire_SearchFirst(&bus,&search,found));
    CHECK(memcmp(rom,found,8)==0 && search.last_device);
    CHECK(!OneWire_SearchNext(&bus,&search,found));
    rom[7]^=1; reset_samples(true);
    for(size_t bit=0;bit<64;++bit) {
        bool value=(rom[bit/8]>>(bit%8))&1;
        samples[length++]=value; samples[length++]=!value;
    }
    CHECK(!OneWire_SearchFirst(&bus,&search,found));
    return 0;
}
