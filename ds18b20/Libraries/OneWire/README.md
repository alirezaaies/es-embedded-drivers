# OneWire

Portable C code; no MCU headers or heap allocation. Copy this folder together with DS18B20 and add both `Inc` paths and `Src/*.c` to your build. The complete callback binding and error-handling example are in [the standalone project guide](../../README.md). One externally powered DS18B20 per bus is required by the temperature API. See the headers for signatures and timing contracts.

## فارسی

کد C قابل‌حمل، بدون هدر MCU و تخصیص حافظهٔ پویا است. هر دو کتابخانه را کپی کنید، مسیرهای Inc و فایل‌های C را به ساخت اضافه کنید. روش کامل اتصال callbackها و کنترل خطا در [راهنمای مستقل پروژه](../../README.md) آمده است. API دما فقط برای یک سنسور با تغذیهٔ مستقل در هر باس است. قرارداد زمان‌بندی و امضای توابع در هدرها مشخص است.
