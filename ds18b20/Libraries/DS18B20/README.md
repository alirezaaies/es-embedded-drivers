# Portable DS18B20

Copy this folder and `OneWire` into your target project. Compile both C files
and OneWire's C file; add both Inc directories. No STM32 dependency or heap.
The addressed API is in `ds18b20.h`; the configurable array API is in
`ds18b20_array.h`. See [project usage](../../README.md) for complete examples,
timing, replacement rules and error handling. Initializing the bus binds GPIO;
initializing the array binds caller-owned storage and starts discovery.

## فارسی

این پوشه را همراه OneWire کپی کنید و فایل‌های C و مسیرهای Inc را به پروژه
اضافه کنید. کتابخانه به میکرو وابسته نیست و حافظهٔ پویا ندارد. راهنمای کامل
ورودی آدرس، خروجی دما، تعویض سنسور و خطاها در ریدمی پروژه آمده است.
