# DS18B20 project status — 2026-10-12

Owner: **Alireza Khajehvandi / علیرضا خواجه‌وندی**.

## Current revision: Keil Watch and beginner teaching — 2026-10-12

The owner reports executing the earlier firmware on the real board in Keil: temperature was visible when halted at the publication breakpoint, but not during free execution. No numerical values, sample counts, accuracy comparison or waveforms were supplied.

### Diagnosis and fix

The local ignored MDK-ARM/DS18B20.uvoptx had DebugFlag/periodic=0. This matches Keil's documented refresh-on-stop behavior; Watch can be stale while the firmware continues reading. The local driver selection is J-Link. The saved periodic flag is now 1 and seven global Watch expressions are present. The user's currently open Keil session retains its in-memory options; View → Periodic Window Update must be enabled in that session too. No claim is made that the active GUI setting or continuous board readings were verified.

The tracked tools/setup_keil_watch.py recreates the local setup, preserving probe choices, existing watches and breakpoints and backing up .uvoptx. A disposable-options test verified refresh=1, preserved probe, exactly seven expected expressions and no changes on a repeated run. Run after closing Keil; if an open session saves older options, rerun afterward. Global variables, not the local value, belong in live Watch. After source changes the owner must rebuild/download the updated binary before using its new symbols.

### Implementation and actual validation

- STM32 callbacks moved into Application/Src/ds18b20_stm32.c. The simple board-facing API is DS18B20_STM32_Init once after HAL/clock/GPIO and DS18B20_STM32_ReadTemperature from foreground code. The portable library and IOC/Core hardware settings are unchanged.
- g_temperature_valid now describes the latest completed attempt and remains visible during the next conversion. g_measurement_busy reports that conversion separately. g_successful_samples counts only successes and g_last_success_ms records the last success tick. An error invalidates the last attempt without replacing the old numeric temperature.
- New emulated application tests verify busy state, visible prior validity during conversion, success/attempt counts, successful timestamps, unchanged temperature/time after error and recovery. All three C suites passed (driver, wire and application).
- Structural checks and Keil synchronization passed. Both PlatformIO probe builds passed, with ststm32 19.6.0 and GCC 7.2.1: 7444 bytes Flash, 1208 bytes reported RAM.
- Actual Keil ARM Compiler 6.24 build of a disposable standalone copy passed with 0 errors / 0 warnings. Code 5312, RO-data 416, RW-data 16, ZI-data 1080. The separate build avoided changing the user's active debug session. No hardware programming was performed by the agent.
- Reports rewritten in matching beginner stages: first wiring/build/download/Watch, then two-function reuse and the runnable example, then protocol/callback details. New editable TikZ/vector cover states the repository's learning and reuse purpose. Final PDFs rebuilt with XeLaTeX/BibTeX: English 20 pages, Persian 19 pages. All pages rendered, contact sheets and representative full-size cover/Watch/API/transition/reference/contact pages visually checked. No overfull boxes, missing glyphs or unresolved references in final logs. Exact author metadata and six footer destinations passed on every page. The cover/contact checks additionally require each author URL in both the body block and footer of the same first/last page, preventing an accidental second cover page.

### Remaining target acceptance

Enable live refresh in the active Keil session; remove transaction breakpoints, download the revised binary and observe global success/attempt counters. Confirm warm/cool response and disconnect/reconnect recovery, then reference accuracy and electrical timing under real interrupt load. The initial reading is owner-reported; continuous operation after this fix has not yet been confirmed.

## Previous revision records

The records below describe the earlier implementation and validation, before the owner's hardware feedback and this Watch/teaching revision. The current section above takes precedence for source paths, variables, evidence and remaining work.

## Objective and current architecture

The owner requested replacing the difficult prior driver and reports with a minimal educational implementation, a runnable board example and one central growing report. Old addressed/array/cooperative APIs and alternative examples were removed. The entire prior project, including the owner's uncommitted changes and local report files, was copied outside this repository to `E:/akh/github/agfa/es-embedded-drivers-backup-20261012/ds18b20` before replacement.

The current portable API is `DS18B20_ReadTemperature(sensor, &celsius)`: one externally powered sensor per bus, blocking conversion polling, full scratchpad CRC, fixed-byte/range validation and signed/resolution decoding. The output changes only on success. No heap, ROM search, alarms, EEPROM writes or parasite-power support. Multiple sensors are a caller-precondition violation; the driver does not count sensors.

Application owns STM32 register/DWT/HAL callbacks and a complete foreground test. `g_temperature_c`, `g_temperature_status`, `g_temperature_valid` and `g_sample_cycles` expose the latest outcome to the debugger. Core only calls the app from USER CODE. PB10 stays open drain, initial high, no pull, external 4.7 kΩ to 3.3 V assumed. HSE 8 MHz / core 72 MHz must be checked on the physical board. The owner's circuit is reported assembled.

Every firmware file is local: IOC, Core, vendor HAL/CMSIS, GNU/Keil startup, linker, build settings, app, libraries and tests. The report moved to repository-root docs with one chapter per project; local README retains full standalone instructions. The project docs directory is only a pointer, with no report PDF. No sibling firmware dependency exists.

## Actual validation for this revision

- Portable C suites passed on emulated Cortex-M3 using ARM GCC and Unicorn. Driver tests cover command sequence, conversion completion/timeout boundary, positive/negative temperatures, all four resolutions, endpoints, absence at both resets, invalid arguments, CRC errors, invalid fixed/config bytes, all-zero data and unchanged output after failures. Wire tests run actual implementation and check presence/idle handling, LSB order, saved IRQ state and published CRC vector.
- Structural checks passed: local Keil sources, memberships, startup, USER CODE hooks, labeled IOC pin/macros, 256 KiB Flash and no MCU includes in portable code.
- Final PlatformIO builds passed for J-Link and ST-Link using ststm32 19.6.0 / ARM GCC 7.2.1, warnings as errors. Final size: 7396 bytes Flash and 1196 bytes reported RAM.
- Actual Keil build passed with ARM Compiler 6.24 and STM32F1xx_DFP 2.4.1: 0 errors, 0 warnings. Program Size: Code 5272, RO-data 416, RW-data 16, ZI-data 1072. Local log is ignored in MDK-ARM/build/validation-final.txt.
- Actual CubeMX 6.17.0 generation was run in a disposable standalone copy, with installed STM32Cube F1 1.8.7. Application hooks survived, application/library files remained intact, sync/check passed and the generated copy built with PlatformIO. Generated Core and updated IOC configuration were adopted and both final firmware builds repeated. Generator emitted internal CodeEngine diagnostics but ended OK; generated artifacts were checked and compiled. An additional PB11 pin-move regeneration passed structural checks and both PlatformIO environments, and actual Keil compilation (0 errors, 0 warnings). The adapter byte-for-byte matched the delivered adapter, and generated macros became GPIO_PIN_11. The main project remains PB10. Installed CubeMX identifies itself in the startup log as 6.17.0-RC5 (updater 6.17.0).
- Final central reports rebuilt with XeLaTeX/BibTeX: English 23 pages, Persian 22 pages. All pages rendered; contact sheets and representative full pages (including cover, contents, source, Persian references and contact) reviewed. No overfull boxes, missing characters or unresolved citations/references in final logs. Author metadata matches each language. All six exact author destinations occur in the footer of every page, with page-bound link rectangles verified. Cover and final contact have the six platform icons and labels. Latin bibliography and URLs render in Latin while Persian page identity remains Persian. Final report PDFs are not ignored; datasheet PDFs, downloaded fonts, renders and probe logs are ignored.

## Integration limits

Read from main/foreground with interrupts enabled. HAL millisecond tick granularity may make an 800 nominal-wait timeout take about 1.7 s plus interrupt overhead. Reset masks interrupts for about 550 μs plus call overhead; write-zero about 60 μs and read about 13 μs. These are requested timings, not measured waveforms. DWT callbacks are specific to this Cortex-M3 example. A valid 85 °C is accepted; persistent implausible readings require power/timing diagnosis. Volatile debugger globals are not an RTOS synchronization mechanism. Serialize access to a physical bus and design application stale-data handling separately.

## Unfinished physical work

No physical programming, temperature samples, reference comparison or scope/logic-analyzer measurements were performed for this revision. Execute README board acceptance: supply/clock/idle checks, first valid reading, warm/cool response, disconnect/reconnect recovery, reference comparison and timings under real interrupt load. Record supply, crystal, pull-up, cable, probe, firmware revision, instruments, samples and failures individually. Never equate emulator or compiler success with board validation.

## Continuation rules

Read AGENTS and every agent-guidelines file. Keep portable libraries MCU-free, Core edits in USER CODE and hardware consistent with IOC. After generation synchronize Keil, run structural checks/tests and rebuild both tools. For a new IC copy the raw template as a standalone project, not a build dependency. Extend both central report editions with a chapter and rebuild/verify PDFs. Official datasheet links/revisions only; no datasheet PDFs, fonts, renders or build products in Git. Exact author branding and all six links remain on cover, every footer and final contact page.

## فارسی

بازنویسی با هدف API ساده، تست اجرایی برد و گزارش جامع مرکزی انجام شد. نسخهٔ قبلی و تغییرات ذخیره‌نشده پیش از جایگزینی خارج مخزن پشتیبان گرفته شدند. تابع فعلی فقط دمای یک سنسور با تغذیهٔ مستقل را می‌خواند. قابلیت‌های چندسنسوره و state machine قبلی حذف شدند. ساخت واقعی کیل و هر دو محیط PlatformIO، تست شبیه‌سازی و بازتولید CubeMX بررسی شدند؛ هنوز تست فیزیکی برد انجام نشده است. ادامهٔ کار باید همین استقلال پروژه، USER CODE، منبع رسمی دیتاشیت و دو فصل متناظر گزارش مرکزی را حفظ کند.

## Completion record

The minimal library, runnable PB10 board example, both build systems, regeneration/pin-transfer checks, standalone READMEs, central bilingual report and future-project workflow are complete. Remaining work is physical acceptance on the assembled target, listed above. No commit or push was performed.
