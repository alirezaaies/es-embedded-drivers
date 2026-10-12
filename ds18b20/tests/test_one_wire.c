#include "one_wire.h"
#define CHECK(x) do { if (!(x)) return __LINE__; } while (0)
static bool samples[80], locked, low_level;
static unsigned at, count, slots, bit_count;
static uint8_t written;
static uint32_t held, max_held, current_low;
static void low(void *c) { (void)c; low_level=true; current_low=0; }
static void release(void *c)
{
    (void)c;
    if (low_level && current_low < 100) {
        if (current_low == 6) written |= (uint8_t)(1U << bit_count);
        ++bit_count; ++slots;
    }
    low_level=false;
}
static bool read_line(void *c) { (void)c; return at < count ? samples[at++] : true; }
static void delay(void *c,uint32_t n)
{ (void)c; if (locked) held+=n; if (low_level) current_low+=n; }
static uint32_t enter(void *c) { (void)c; locked=true; held=0; return 7; }
static void leave(void *c,uint32_t s)
{ (void)c; if(s!=7) max_held=9999; if(held>max_held) max_held=held; locked=false; }
static void presence(bool found)
{ at=0; count=3; samples[0]=true; samples[1]=!found; samples[2]=true; }
int main(void)
{
    OneWire_Bus b={0,low,release,read_line,delay,enter,leave};
    CHECK(OneWire_IsValid(&b));
    CHECK(!OneWire_IsValid(0));
    b.exit_critical=0; CHECK(!OneWire_IsValid(&b)); b.exit_critical=leave;
    presence(true); CHECK(OneWire_Reset(&b) && max_held==550 && !locked);
    presence(false); CHECK(!OneWire_Reset(&b));
    presence(true); samples[0]=false; CHECK(!OneWire_Reset(&b));
    presence(true); samples[2]=false; CHECK(!OneWire_Reset(&b));
    written=0; slots=bit_count=0; max_held=0;
    OneWire_WriteByte(&b,0xa5);
    CHECK(written==0xa5 && slots==8 && max_held==60 && !locked);
    at=0; count=8; bit_count=0; max_held=0;
    for(unsigned i=0;i<8;++i) samples[i]=((0x96U>>i)&1)!=0;
    CHECK(OneWire_ReadByte(&b)==0x96 && at==8 && max_held==13 && !locked);
    const uint8_t bytes[8]={0x50,0x05,0x4b,0x46,0x7f,0xff,0x0c,0x10};
    CHECK(OneWire_Crc8(bytes,8)==0x1c);
    CHECK(OneWire_Crc8(bytes,0)==0);
    return 0;
}
