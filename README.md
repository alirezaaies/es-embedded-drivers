# Embedded drivers

Author: **Alireza Khajehvandi**.

I keep each sensor or IC as a standalone project, with a small portable C library and a runnable hardware example. The same repository is also a practical learning guide.

| Project | Hardware example | Current scope | Validation |
| --- | --- | --- | --- |
| [DS18B20](ds18b20/README.md) | STM32F107VCT6, PB10 | One externally powered sensor, blocking temperature read | PlatformIO, Keil and portable tests; board acceptance pending |
| [STM32F107 foundation](templates/stm32f107/README.md) | STM32F107VCT6 | Copy as the starting point for a new project | See its local status |

The [central report](docs/README.md) grows by adding one chapter per project. Editable LaTeX and final [English](docs/english/main_report.pdf) and [Persian](docs/persian/main_report.pdf) PDFs live only under root `docs/`.

Each project owns its IOC, Keil, PlatformIO, HAL/CMSIS, application and libraries. Copy the whole project to work independently, or copy only portable libraries and implement your own board callbacks. No project builds against a sibling or the raw template. Datasheets remain local and ignored; official source links and revisions appear in each README.

For a new driver, follow [the extension guide](agent-guidelines/project_workflow.md) and record actual checks in `PROJECT_STATUS.md`. Board measurements are recorded separately from builds and simulated tests.

## کتابخانه‌های سیستم‌های نهفته

نویسنده: **علیرضا خواجه‌وندی**.

هر سنسور یا آی‌سی یک پروژهٔ مستقل دارد: کتابخانهٔ کوچک C، نمونهٔ اجرایی برای تست برد، IOC، کیل، PlatformIO و فایل‌های vendor. برای استفادهٔ مستقل کل پوشهٔ پروژه را کپی کنید؛ برای انتقال کتابخانه به برد دیگر فقط کتابخانه‌های قابل‌حمل و توابع اتصال به سخت‌افزار لازم‌اند.

پروژهٔ فعلی [DS18B20](ds18b20/README.md) دمای یک سنسور با تغذیهٔ مستقل را روی PB10 می‌خواند. ساخت نرم‌افزار و تست شبیه‌سازی با نتیجهٔ سخت‌افزار یکسان نیست؛ تست واقعی برد باید ثبت شود.

[گزارش جامع](docs/README.md) در docs ریشه است و با افزودن فصل هر پروژه توسعه می‌یابد. سورس قابل ویرایش و PDF نهایی هر دو زبان نگهداری می‌شوند. دیتاشیت‌ها وارد گیت نمی‌شوند و منبع رسمی و نسخهٔ آن در README هر پروژه مشخص است. برای پروژهٔ بعدی [راهنمای توسعه](agent-guidelines/project_workflow.md) را دنبال کنید.
