# Portable standard-speed 1-Wire

Supply drive-low, release, read, microsecond-delay and paired critical callbacks.
Release means high impedance through open-drain; never actively drive high.
Delay must keep working with interrupts disabled. Lock returns prior interrupt
state; unlock restores it. ROM search includes Dallas CRC8. Transactions are
synchronous; conversion scheduling belongs to DS18B20_Array. See ../../README.md.

## فارسی

callbackهای پایین‌کشیدن، رهاکردن، خواندن، تأخیر دقیق و ورود/خروج ناحیهٔ حساس
را فراهم کنید. خروجی high باید رهاکردن open-drain باشد. تأخیر با وقفهٔ بسته
هم باید کار کند و خروج ناحیهٔ حساس وضعیت قبلی وقفه را برگرداند.
