# DS18B20 portable driver and STM32F107 example

Author: **Alireza Khajehvandi**. This folder is a standalone project: copy it alone.
It contains CubeMX configuration, Keil project, PlatformIO, vendor HAL/CMSIS,
portable drivers, tests and editable English/Persian reports.

## Build and program

1. Install PlatformIO (VS Code extension or `python -m pip install platformio`).
2. Open this folder, then run `pio run` (J-Link) or `pio run -e stm32f107vc_stlink`.
3. Connect SWDIO=PA13, SWCLK=PA14, GND and target voltage reference to the probe.
4. Program with `pio run -t upload` or `pio run -e stm32f107vc_stlink -t upload`.
5. In Keil, open `MDK-ARM/DS18B20.uvprojx`, install the STM32F1 device pack
   and Arm Compiler 6, select your SWD probe in Options/Debug and Utilities,
   select STM32F10x 256 KiB Flash algorithm, Build, then Download.
   Keil build and physical programming have not been performed here.
6. Watch `g_sensor_roms`, `g_temperature_c`, `g_temperature_status`,
   and `g_sample_cycles`. Use temperature only when status is `DS18B20_STATUS_OK`.

PB10 is the only sensor bus. Connect DQ to PB10, a typical 4.7 kOhm pull-up to
3.3 V, VDD to 3.3 V and common GND. Confirm your packaged sensor's pinout;
probe cable colours are not standardized. This implementation requires external
power; parasite power and its strong pull-up are not implemented. The copied
baseline specifies an 8 MHz HSE crystal, 72 MHz core and 3.3 V supply. These are
configuration evidence, not measurements of your circuit.

## Simple use

Set `APP_SENSOR_COUNT` in `Application/Inc/app.h` (default 4). Use 1 for one
sensor. Empty ROM slots are discovered automatically. Alternatively initialize
`g_sensor_roms` with your own eight-byte addresses in wire order (family first,
CRC last). The output arrays retain the same order as the ROM array.

```c
DS18B20_Rom addresses[N] = {{0}}; /* Or supply your known ROMs. */
DS18B20_Rom discovered[N];
float temperatures[N];
DS18B20_Status status[N];
DS18B20_Array sensors;
/* bus is initialized using your GPIO/time/critical-section callbacks. */
DS18B20_ArrayInit(&sensors, &bus, addresses, discovered, temperatures, status, N);
/* In the main loop: */
if (DS18B20_ArrayProcess(&sensors, milliseconds())) {
    /* One cycle complete. Inspect status[i] before temperatures[i]. */
}
```

For one explicit address, call `DS18B20_ReadTemperatureBlocking(&bus, rom,
1000, &temperature)` for a complete blocking measurement, or
`DS18B20_StartConversion(&bus, rom)` followed after conversion by
`DS18B20_ReadTemperature(&bus, rom, &temperature)` for the nonblocking flow.
The latter reads a completed conversion, not a new measurement. Never send
another bus command while a conversion is being polled. `DS18B20_WriteScratchpad`
sets volatile resolution and alarm thresholds; this example uses the conservative
12-bit conversion budget and does not write sensor EEPROM.

## Replacement and timing

Discovery runs approximately every two seconds. With exactly one missing
assigned ROM and exactly one unknown ROM, the new ROM takes the old index.
Multiple missing/new devices are ambiguous: keep mappings and replace sensors
one at a time, or call `DS18B20_ArrayReplace` while `sensors.phase == 0`.
CRC/protocol failures or a discovery buffer overflow inhibit automatic remapping.
ROM changes are in RAM only; preserve your address array in application-owned
storage if identities must survive reset. No Flash is reserved by this example.
The discovery buffer must have the same capacity as the configured slot count;
raise it by increasing that count if more sensors are physically connected.

A configurable 250 ms idle gap (`sensors.sample_interval_ms`) limits retries
and CPU use between cycles. Conversion timing starts on the call after the
command has returned, so command-time interrupts cannot shorten the wait.

Conversion wait (760 ms including command/tick margin) is cooperative. Each
process call performs at most one search (~15 ms) or one addressed read (~12 ms)
at standard speed. GPIO critical sections cover reset sampling (~70 us), write
zero (~60 us), write one (~6 us), and read sampling (~13 us), plus callback
overhead. Interrupts run during recovery and conversion. This is not suitable
unchanged for hard interrupt deadlines below those durations; use a hardware
1-Wire master or timer/DMA transport for such systems. Do not call from an ISR,
do not share a bus between tasks without serialization, and avoid sleep/clock
changes during transactions. DWT timing is specific to this Cortex-M3 adapter;
other MCUs supply a timer-based delay callback.

During conversion outputs become `NAN` and status `BUSY`; failed reads stay
`NAN`. A valid 85 C sample is accepted: power-on 85 C cannot be distinguished
from a real 85 C by value alone. Array mode starts and waits for conversion
before reading. CRC8 checks ROMs and all nine scratchpad bytes; lower-resolution
undefined temperature bits are masked and out-of-range values are rejected.

## CubeMX regeneration

Open `DS18B20.ioc`. PB10 must remain output open-drain, initially high, no
internal pull-up. Keep User Code is enabled. Application and Libraries live
outside generated Core; includes and App calls are inside USER CODE sections.
The `.ioc` names `tools/post_generate.cmd` as its post-generation hook. If your
CubeMX version does not resolve the relative path, select that script explicitly
in Project Manager / Advanced Settings / User Actions. After Generate always run:

```text
python tools/sync_keil.py
python tools/check_project.py
pio run
```

The idempotent sync restores custom source groups, include paths, startup and
full Flash size in Keil. PlatformIO selects local Core, HAL and Libraries using
wildcards; for newly enabled peripherals review `stm32f1xx_hal_conf.h` and
Keil HAL source membership. The script discovers HAL sources automatically.
CubeMX must still supply any required interrupt/MSP initialization.
Review the diff and build Keil too. Actual CubeMX regeneration has not been run
in this environment; structural checks are not a substitute for it.

## Tests and reports

Run `python -m pip install unicorn`, then `python tests/run_tests.py`.
Tests execute actual portable C on a Cortex-M3 emulator with simulated bus
responses. They cover CRC, signed temperature, resolution, ordering, safe
replacement, ambiguous/incomplete search, overflow, absent/stuck lines, timeout,
tick wrap and requested critical-section delays. They do not measure real GPIO
timing, wiring or interrupt latency. See [PROJECT_STATUS.md](PROJECT_STATUS.md)
for evidence, and [docs/README.md](docs/README.md) for editable LaTeX/PDF reports.
Original datasheet links are in [datasheets/README.md](datasheets/README.md).
Vendor license notices remain in Drivers. First-party code follows this folder's
[LICENSE](LICENSE); preserve third-party licenses when copying.

---

# کتابخانهٔ قابل‌حمل DS18B20 و نمونهٔ STM32F107

نویسنده: **علیرضا خواجه‌وندی**. این پوشه مستقل است و به هیچ فایل پوشهٔ همسایه
نیاز ندارد. تنظیمات CubeMX، پروژهٔ Keil، PlatformIO، فایل‌های HAL و CMSIS،
کتابخانه‌ها، آزمون‌ها و گزارش‌های قابل‌ویرایش داخل همین پوشه قرار دارند.

## اجرا و مشاهدهٔ دما

۱. PlatformIO را نصب و همین پوشه را باز کنید. دستور `pio run` برای ساخت و
`pio run -t upload` برای پروگرام با J-Link است. برای ST-Link از محیط
`stm32f107vc_stlink` در دستورهای بالا استفاده کنید.
۲. در Keil فایل `MDK-ARM/DS18B20.uvprojx` را باز کنید، پکیج STM32F1 و کامپایلر
Arm Compiler 6 را نصب کنید، پروگرامر SWD و الگوریتم Flash با ظرفیت ۲۵۶ کیلوبایت
را انتخاب کنید و Build و Download را بزنید. ساخت واقعی Keil هنوز انجام نشده است.
۳. در Watch آرایه‌های `g_sensor_roms`، `g_temperature_c` و
`g_temperature_status` را ببینید. دما تنها با وضعیت `DS18B20_STATUS_OK` معتبر است.
۴. تعداد سنسورها را با `APP_SENSOR_COUNT` در `Application/Inc/app.h` تعیین کنید؛
برای یک سنسور عدد ۱ را قرار دهید. ورودی آدرس و خروجی دما هم‌ترتیب هستند.

خط داده به PB10 متصل است؛ مقاومت pull-up معمولاً ۴٫۷ کیلو‌اهم به ۳٫۳ ولت،
تغذیهٔ خارجی سنسور ۳٫۳ ولت و زمین مشترک لازم است. ترتیب پایه را از دیتاشیت
بسته‌بندی بررسی کنید. تغذیهٔ parasite پشتیبانی نشده است. کلاک ۸ مگاهرتز خارجی
و فرکانس هستهٔ ۷۲ مگاهرتز از تنظیمات پروژهٔ قبلی استخراج شده‌اند؛ اندازه‌گیری
سخت‌افزاری انجام نشده است.

## استفادهٔ کتابخانه و تعویض سنسور

نمونهٔ کد بخش انگلیسی رابط آرایه‌ای را نشان می‌دهد. با `DS18B20_ArrayInit`
آرایهٔ آدرس‌ها، بافر جست‌وجو، آرایهٔ دما، آرایهٔ وضعیت و تعداد را معرفی کنید.
آدرس صفر یعنی خانهٔ خالی برای کشف خودکار. آدرس معلوم باید شامل هشت بایت،
از family تا CRC باشد. `DS18B20_ArrayProcess` را مرتب در حلقهٔ اصلی صدا بزنید؛
انتظار تبدیل دما مسدودکننده نیست. اگر آدرس واحد دارید، تابع
`DS18B20_ReadTemperatureBlocking` اندازه‌گیری کامل ولی مسدودکننده انجام می‌دهد؛
روش مناسب برنامهٔ بزرگ، شروع تبدیل و سپس خواندن با `DS18B20_ReadTemperature` است.
این تابع آخر فقط نتیجهٔ تبدیل قبلی را می‌خواند. تنظیم رزولوشن و آلارم با
`DS18B20_WriteScratchpad` موقتی است و در EEPROM نوشته نمی‌شود.

اگر دقیقاً یک سنسور تعریف‌شده غایب و دقیقاً یک آدرس جدید حاضر باشد، سنسور جدید
در همان خانه جایگزین می‌شود. در حالت مبهم، سنسورها را یکی‌یکی تعویض کنید یا
در حالت idle از `DS18B20_ArrayReplace` استفاده کنید. جست‌وجوی ناقص و سرریز بافر
اجازهٔ نگاشت خودکار نمی‌دهند. نگاشت فقط در RAM است و پس از ریست باقی نمی‌ماند؛
برای ماندگاری، ذخیره‌سازی را در برنامهٔ خود اضافه کنید. هنگام تبدیل وضعیت
`BUSY` و مقدار `NAN` است؛ خطا نیز مقدار معتبر قبلی را دوباره ارائه نمی‌کند.

هر بار پردازش حداکثر یک جست‌وجوی حدود ۱۵ میلی‌ثانیه یا یک خواندن حدود ۱۲
میلی‌ثانیه انجام می‌دهد. انتظار تبدیل ۷۶۰ میلی‌ثانیه‌ای آزاد است، ولی تبادل
1-Wire کاملاً بدون مصرف CPU نیست. وقفه‌ها در بازه‌های حساس تقریباً ۶ تا ۷۰
میکروثانیه، به‌علاوهٔ سربار callback، بسته می‌شوند. برای مهلت‌های سخت کوتاه‌تر،
لایهٔ انتقال سخت‌افزاری لازم است. از ISR صدا نزنید و دسترسی چند task به باس
را سری کنید. برای میکروهای دیگر فقط callbackهای GPIO، تأخیر دقیق و حفاظت وقفه
را تغییر دهید؛ کتابخانه هیچ هدر STM32 ندارد.

## تولید مجدد و آزمون

در CubeMX گزینهٔ Keep User Code فعال و PB10 خروجی open-drain با حالت اولیهٔ
high است. کدهای برنامه بیرون از Core و فراخوانی‌ها در USER CODE هستند.
پس از Generate، سه دستور sync، check و build بخش انگلیسی را اجرا کنید.
اگر مسیر نسبی اسکریپت پس‌تولید شناخته نشد، آن را در تنظیمات User Actions
انتخاب کنید. افزودن peripheral مستلزم بررسی HAL، وقفه‌ها و MSP نیز هست.
تولید واقعی CubeMX و ساخت Keil در این محیط تأیید نشده‌اند.

با نصب unicorn و اجرای `python tests/run_tests.py` آزمون‌های C روی شبیه‌ساز
Cortex-M3 اجرا می‌شوند. آزمون برد، زمان‌بندی واقعی پایه و پروگرام واقعی هنوز
لازم است. راهنمای گزارش و منابع در پیوندهای بخش انگلیسی قرار دارد.
