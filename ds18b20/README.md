# DS18B20: a simple temperature reader

Author: **Alireza Khajehvandi**.

This standalone project reads **one externally powered DS18B20** on PB10 using STM32F107VCT6. The driver exposes one blocking function. It starts a new conversion, waits for completion, reads all nine scratchpad bytes, checks CRC and returns Celsius. It does not change the sensor configuration. Multiple sensors, ROM discovery, parasite power and alarms are outside this version's scope.

## 1. Wire the board

| Signal | Connection |
| --- | --- |
| DS18B20 VDD | 3.3 V |
| DS18B20 GND | MCU ground |
| DS18B20 DQ | PB10 (`ONE_WIRE_1`) |
| Pull-up | 4.7 kΩ between DQ and 3.3 V; verify rise time on your actual wiring |
| SWD | PA13/SWDIO, PA14/SWCLK, ground and reference voltage; NRST if needed |
| Clock | 8 MHz external crystal, PLL ×9, 72 MHz core |

Check your package pin view against the datasheet, especially TO-92 bottom/top view. GPIO is open drain, initially released, without internal pull-up. Use external VDD, not parasite power. Confirm the physical crystal matches the IOC before programming.

Official reference: [↗ Analog Devices / Maxim DS18B20 datasheet, 19-7487, Rev 6, July 2019](https://www.analog.com/media/en/technical-documentation/data-sheets/ds18b20.pdf). Datasheet PDFs are not tracked.

## 2. Build

Open this folder in PlatformIO, install the PlatformIO Core/VS Code extension, then run from this folder:

```sh
pio run -e stm32f107vc_jlink -e stm32f107vc_stlink
python tools/check_project.py
python tests/run_tests.py
```

Tests need Python `unicorn` and the ARM GCC package installed by PlatformIO (`python -m pip install unicorn`). If `pio` is not on PATH, use your PlatformIO installation's executable. The checked platform is `ststm32@19.6.0`. Builds use local HAL/CMSIS rather than a downloaded framework.

For Keil, open `MDK-ARM/DS18B20.uvprojx`, install `Keil.STM32F1xx_DFP` 2.4.1 and select ARM Compiler 6 (verified with 6.24). Run `python tools/sync_keil.py` before building. Choose your actual debug adapter in Options for Target → Debug/Utilities. The project includes its own startup and sources.

## 3. Program and observe temperature

```sh
pio run -e stm32f107vc_jlink -t upload
# Or use -e stm32f107vc_stlink with an ST-Link probe.
pio debug -e stm32f107vc_jlink
```

In Keil use Download, then Start/Stop Debug Session and Run. Add these variables to Watch:

| Variable | Meaning |
| --- | --- |
| `g_temperature_c` | Last successful Celsius reading; starts at zero, which is not a measurement |
| `g_temperature_status` | 0 OK, 1 invalid argument, 2 no presence/bus held low, 3 timeout, 4 CRC error, 5 invalid data |
| `g_temperature_valid` | Result of the latest completed attempt; stays visible during a new conversion and becomes false after error |
| `g_measurement_busy` | True while a new read is in progress |
| `g_sample_cycles` | Number of completed attempts, including failures |
| `g_successful_samples` | Successful readings; confirms progress even when temperature stays constant |
| `g_last_success_ms` | HAL tick at the last successful reading |

**In the active Keil debug session, enable View → Periodic Window Update.** Without it, Watch refreshes when execution stops, so a breakpoint can reveal a value that was already being measured. Use the global `g_temperature_c`, not the local `value`. See the [↗ official Keil Watch documentation](https://www.keil.com/support/man/docs/uv4cl/uv4cl_db_dbg_watchwin.htm).

The saved local options had `periodic=0`; they were changed to `1` and all seven global expressions were added. An already open session keeps its loaded settings: enable the menu there too. The local `.uvoptx` is ignored in Git. For a reproducible setup, save/close Keil and run `python tools/setup_keil_watch.py`, then reopen the project. The tool preserves probe selection, other watches and breakpoints. If an open session saves older options, rerun it after closing Keil.

After code changes, leave debug, rebuild and download the new firmware before watching new variables. Run continuously. Do not halt/step in the middle of 1-Wire transactions: debugger pauses break timing. If variables are optimized away in PlatformIO, use `pio debug`, or set `build_type = debug` under `[env]` in `platformio.ini` and rebuild.

The example calls `App_Process()` from the main loop, then pauses 250 ms. It blocks while converting; it is not a periodic real-time scheduler. The portable driver allows 800 nominal 1 ms waits; the STM32 HAL adds tick granularity, so a timeout can take roughly 1.7 s plus interrupt overhead. Do not call it from an ISR or with interrupts disabled. A reset masks interrupts for approximately 550 μs plus overhead; consider this before using it in a control loop.

Board acceptance procedure:

1. Check idle DQ near 3.3 V and the real HSE frequency.
2. Run the firmware; check status 0, valid true and increasing attempt count.
3. Warm the sensor gently; confirm successive readings rise and cool again.
4. Disconnect DQ; confirm a failure and valid false. The old temperature stays in memory and must not be used.
5. Reconnect one sensor; confirm successful readings resume without reset.
6. Compare against a reference thermometer and inspect DQ with a logic analyzer under representative interrupt load. Record supply, clock, cable length, pull-up, firmware revision and observed waveforms/results in `PROJECT_STATUS.md`.

The owner reports that the earlier firmware ran in Keil and temperature was visible at a breakpoint. The corrected continuous Watch setup and this updated firmware still need target verification; accuracy and waveforms have not been measured here.

## 4. Use the library in another project

For the same STM32F107, copy both portable library folders plus `Application/Inc/ds18b20_stm32.h` and `Application/Src/ds18b20_stm32.c`. Compile the three C files and add their three Inc paths. Keep the generated open-drain GPIO label `ONE_WIRE_1`.

```c
#include "ds18b20_stm32.h"
/* Once after HAL, clock and GPIO initialization: */
DS18B20_STM32_Init();
/* From the foreground loop, with interrupts enabled: */
float temperature;
DS18B20_Status status = DS18B20_STM32_ReadTemperature(&temperature);
if (status == DS18B20_OK) {
    /* Use temperature here. It is unchanged on error. */
}
```

The return value is the **status**, while `&temperature` gives the function the address where it writes Celsius. The output changes only on success. Place each snippet in its proper include/init/loop area. Use either your own loop or the supplied `App_Process`, not two readers at once.

For another MCU, copy only the two portable library folders and implement the callbacks in their headers. `Application/Src/ds18b20_stm32.c` demonstrates them for Cortex-M3. The portable API remains `DS18B20_ReadTemperature(&sensor, &temperature)`. Release means open-drain high impedance; microsecond delays must be accurate, millisecond waits must wait at least the requested interval, and critical callbacks must restore the saved interrupt state. The adapter contains MCU headers; the portable libraries do not.

A CRC-valid 85 °C is accepted because it may be a real temperature. If it persists implausibly, check power and conversion timing; do not hardcode rejection of 85 °C. The single-sensor assumption is a caller precondition, not an automatic count check. Serialize callers on the same bus. Older array/addressed APIs and examples were removed deliberately.

## 5. Change a pin and regenerate

Open `DS18B20.ioc` in CubeMX. Move the GPIO while keeping the **ONE_WIRE_1** label, open-drain output, initial high/released state, no pull and high speed. Keep SWD and the clock configuration correct. Project Manager must keep user code, copy required vendor files, use project name DS18B20 and MDK-ARM V5. Generate code into this same project folder.

Application hooks belong only in `USER CODE` blocks: `#include "app.h"` in Includes, `App_Init();` in block 2 and `App_Process();` in block 3. All driver and board logic lives outside generated Core. `tools/post_generate.cmd` synchronizes the Keil source list and runs checks; if the hook fails or Python is not on CubeMX's PATH, run:

```sh
python tools/sync_keil.py
python tools/check_project.py
pio run -e stm32f107vc_jlink -e stm32f107vc_stlink
```

Build Keil again too. The adapter uses generated pin macros, so no PB10 literal must be changed in it. On a different MCU replace the DWT/register adapter and supply that MCU's vendor/startup/linker files; portable libraries remain unchanged. Test scripts and firmware work without any sibling folder. The optional central report links require the full repository.


## فارسی: خواندن سادهٔ دما

نویسنده: **علیرضا خواجه‌وندی**.

این پروژه مستقل، دمای **یک DS18B20 با تغذیهٔ مستقل** را روی PB10 و STM32F107VCT6 می‌خواند. فقط یک تابع برای خواندن دما دارد؛ شروع تبدیل، انتظار، خواندن scratchpad و بررسی CRC در همان تابع انجام می‌شود. تنظیمات سنسور تغییر نمی‌کند. چند سنسور، جست‌وجوی ROM، آلارم و تغذیهٔ انگلی در این نسخه پیاده‌سازی نشده‌اند.

### اتصال و اجرای تست

VDD را به ۳٫۳ ولت، GND را به زمین مشترک و DQ را به PB10 وصل کنید. مقاومت ۴٫۷ کیلو اهم بین DQ و ۳٫۳ ولت لازم است؛ مناسب‌بودن آن را با زمان بالارفتن سیگنال روی مدار واقعی بررسی کنید. پایه‌های SWD برابر PA13 و PA14 هستند. کلاک فعلی کریستال خارجی ۸ مگاهرتز و کلاک هسته ۷۲ مگاهرتز است؛ قبل از پروگرام با برد تطبیق دهید. جهت پایه‌های پکیج را از [↗ دیتاشیت رسمی، Rev 6، ژوئیهٔ ۲۰۱۹](https://www.analog.com/media/en/technical-documentation/data-sheets/ds18b20.pdf) کنترل کنید؛ PDF دیتاشیت وارد گیت نمی‌شود.

پوشه را در PlatformIO باز کنید و دستورهای بخش Build را اجرا کنید. تست نرم‌افزاری به پایتون و unicorn نیاز دارد؛ نصب PlatformIO ابزار ARM GCC را فراهم می‌کند. در کیل فایل `MDK-ARM/DS18B20.uvprojx` را باز کنید؛ Compiler 6 و پک STM32F1xx_DFP نسخهٔ 2.4.1 لازم است. قبل از Build ابزار sync_keil را اجرا کنید و پروگرامر واقعی خود را در تنظیمات Debug/Utilities انتخاب کنید.

در جلسۀ فعال دیباگ کیل، گزینهٔ **View → Periodic Window Update** را فعال کنید. اگر خاموش باشد، Watch هنگام اجرا ثابت می‌ماند و با breakpoint تازه می‌شود؛ این به معنی نیاز سنسور به breakpoint نیست. تنظیم ذخیره‌شده periodic=0 بود و به 1 تغییر کرد، ولی جلسۀ باز کیل تنظیم قبلی را در حافظه دارد و باید همان گزینهٔ منو فعال شود. فقط g_temperature_c سراسری را ببینید، نه value محلی. برای تکرار تنظیم، کیل را ذخیره و ببندید و `python tools/setup_keil_watch.py` را اجرا کنید. تنظیمات پروگرامر و Watchهای قبلی حفظ می‌شوند. بعد از تغییر کد، از دیباگ خارج شوید، Build و Download جدید انجام دهید.

در Watch متغیرهای جدول بالا را ببینید. وضعیت صفر و valid برابر true یعنی آخرین تلاش کامل موفق بوده است. صفر اولیهٔ دما اندازه‌گیری نیست. g_sample_cycles تمام تلاش‌های کامل و g_successful_samples فقط خواندن موفق را می‌شمارد؛ شمارندهٔ موفق حتی با دمای ثابت باید زیاد شود. g_measurement_busy تبدیل جاری را جدا نشان می‌دهد. اعتبار آخرین نتیجه هنگام تبدیل بعدی حفظ و پس از خطا false می‌شود. توقف وسط تراکنش زمان‌بندی باس را خراب می‌کند.

سنسور را کمی گرم کنید و تغییر دما را ببینید. با قطع DQ باید خطا و valid برابر false شود؛ مقدار قبلی در خطا قابل استفاده نیست. با وصل یک سنسور، خواندن باید برگردد. با دماسنج مرجع و logic analyzer بررسی و نتایج واقعی را در PROJECT_STATUS ثبت کنید. صاحب پروژه اجرای firmware قبلی و دیدن دما هنگام breakpoint را گزارش کرده است. نمایش پیوسته با تنظیم اصلاح‌شده و این firmware جدید هنوز باید روی برد امتحان شود؛ دقت و شکل موج اینجا اندازه‌گیری نشده‌اند.

### انتقال و تغییر پایه

برای STM32F107 دیگر، دو کتابخانه را همراه فایل‌های ds18b20_stm32.h و ds18b20_stm32.c کپی کنید. سه فایل C و سه مسیر Inc را به Build بدهید؛ برچسب GPIO برابر ONE_WIRE_1 باشد. یک بار پس از تنظیم برد DS18B20_STM32_Init و در حلقه DS18B20_STM32_ReadTemperature را طبق مثال بالا فراخوانی کنید. مقدار برگشتی وضعیت است؛ دما از طریق آدرس متغیر خروجی نوشته می‌شود. تست در app.c و اتصال سخت‌افزار در ds18b20_stm32.c جدا هستند. برای MCU دیگر فقط دو کتابخانهٔ قابل‌حمل را بردارید و callback مناسب بنویسید. از حلقهٔ خودتان یا App_Process استفاده کنید، نه هر دو با هم.

تابع مسدودکننده است؛ HAL_Delay به علت دقت تیک می‌تواند زمان timeout را حدود ۱٫۷ ثانیه کند. آن را در ISR یا با وقفهٔ غیرفعال صدا نزنید. هنگام reset وقفه‌ها حدود ۵۵۰ میکروثانیه به‌اضافهٔ سربار بسته‌اند؛ قبل از استفاده در حلقهٔ صنعتی این موضوع را بررسی کنید. مقدار ۸۵ درجه اگر CRC درست باشد پذیرفته می‌شود؛ در صورت تکرار نامعقول، تغذیه و زمان تبدیل را بررسی کنید.

برای تغییر پایه، IOC را باز کنید، برچسب ONE_WIRE_1 را حفظ کنید و GPIO را open drain با مقدار اولیهٔ high، بدون pull و سرعت high تنظیم کنید. کد فقط در USER CODE تغییر کند: include در Includes، App_Init در بلوک 2 و App_Process در بلوک 3. بعد از Generate، sync_keil، check_project و ساخت هر دو ابزار را انجام دهید. آداپتر از ماکروهای تولیدشده استفاده می‌کند و با تغییر GPIO به ویرایش دستی PB10 نیاز ندارد. در MCU دیگر آداپتر DWT، فایل‌های vendor، startup و linker باید متناسب با همان MCU باشند.
