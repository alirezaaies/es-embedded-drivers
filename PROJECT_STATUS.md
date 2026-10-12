# Repository status — 2026-10-12

Independent driver projects share one central bilingual learning report. DS18B20 now has a two-function STM32 adapter, a clear board demo and a beginner sequence from first reading to reuse and implementation. Current evidence and remaining target work are in [the local status](ds18b20/PROJECT_STATUS.md). The [report](docs/README.md) has an editable vector cover for the whole repository and one chapter per driver.

The owner reports a breakpoint-visible temperature on the board. Saved Keil periodic Watch refresh was disabled and has been enabled in local options; the active session still needs the menu option. The updated firmware builds in Keil/PlatformIO and all three emulated suites pass. Continuous target verification and reference/waveform measurements remain pending. Final report QA passed: English 20 pages, Persian 19 pages. All pages rendered; cover, Watch/API and section transitions reviewed. Exact authors, six footer URLs on every page and complete cover/final-contact blocks passed.

Follow AGENTS and every agent-guidelines file. Portable code stays MCU-free; generated edits stay inside USER CODE. Copy the raw template instead of depending on it. Record official datasheet links/revisions and separate compilation/simulation from board evidence. New device chapters begin with an observable practical result before introducing callbacks and source details.

## فارسی

هر درایور پروژه‌ای مستقل و گزارش جامع یک فصل متناظر در دو زبان دارد. نمونهٔ DS18B20 اکنون راه‌اندازی و خواندن سادهٔ STM32، متغیرهای روشن برای مشاهده و آموزش مرحله‌ای دارد. صاحب پروژه دیدن دما هنگام breakpoint را گزارش کرده است. گزینهٔ refresh دوره‌ای ذخیره‌شدهٔ کیل خاموش بود و اصلاح شد؛ در جلسۀ باز نیز باید از منو فعال شود. ساخت و سه تست شبیه‌سازی موفق‌اند؛ اجرای پیوستهٔ این اصلاح و دقت و موج واقعی هنوز باید بررسی شوند. مراحل عملی پیش از جزئیات callback و پروتکل آموزش داده می‌شوند.
