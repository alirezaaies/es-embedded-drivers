# Raw STM32F107 standalone template

Author: Alireza Khajehvandi. Copy this entire folder to your next sensor project.
No sensor logic, Flash persistence, LCD or relay control is included. Vendor
HAL/CMSIS is deliberately retained so Keil and PlatformIO share local sources.
Open STM32F107_Template.ioc in CubeMX or MDK-ARM/STM32F107_Template.uvprojx
in Keil. Build with `pio run`; upload with `pio run -t upload` (J-Link) or
`pio run -e stm32f107vc_stlink -t upload`. Configure the Keil SWD probe and
256 KiB Flash algorithm. HSE is 8 MHz; core 72 MHz; MCU STM32F107VCT6.
Write application code in Application and portable code in Libraries.
After CubeMX Generate run `python tools/sync_keil.py`, then build both tools.
Keep App includes/calls in USER CODE blocks. Add each new library's Inc path
to platformio.ini; sync discovers Keil sources/includes and HAL files.
Read agent-guidelines before extending the project. Create docs/english and
docs/persian reports and PROJECT_STATUS.md for each new sensor project.

## قالب خام مستقل STM32F107

نویسنده: علیرضا خواجه‌وندی. کل این پوشه را برای سنسور بعدی کپی کنید. فقط پایهٔ
توسعه، کلاک و SWD باقی مانده‌اند. فایل‌های سازنده برای استقلال پروژه حفظ شده‌اند.
کد کاربردی را در Application و کتابخانه را در Libraries قرار دهید. پس از
تولید CubeMX اسکریپت sync را اجرا و هر دو ابزار را Build کنید. مسیر Inc هر
کتابخانهٔ تازه را به PlatformIO اضافه کنید. دستورهای ساخت و پروگرام در بالا آمده‌اند.
