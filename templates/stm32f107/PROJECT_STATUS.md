# Raw template status — 2026-10-10

Author: Alireza Khajehvandi / علیرضا خواجه‌وندی.

Independent STM32F107VCT6 development template, HSE 8 MHz, core 72 MHz,
256 KiB Flash and 64 KiB RAM. Includes local HAL/CMSIS, GNU/Arm startup,
CubeMX IOC with KeepUserCode, Keil, PlatformIO J-Link/ST-Link environments,
empty Application hooks, synchronization/check scripts and local guidelines.
No sensor, LCD, relay or Flash persistence logic.

PlatformIO build and structural checks pass. Actual Keil compilation,
CubeMX regeneration and physical programming remain unverified. Copy this
entire folder for the next project; never reference sibling driver sources.
Add the new library, pin configuration and include paths, run sync/check,
then build both tools and create local bilingual reports under docs.

قالب خام مستقل آماده و ساخت PlatformIO و بررسی ساختاری موفق است. کد سنسوری
ندارد. تولید واقعی CubeMX، ساخت Keil و آزمون برد باید در پروژهٔ بعدی انجام
و ثبت شوند. برای توسعه همهٔ راهنماهای محلی را بخوانید.
