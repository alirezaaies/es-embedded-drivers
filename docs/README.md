# Central learning report

Author: **Alireza Khajehvandi**.

One complete report grows with the repository: [English PDF](english/main_report.pdf) and [Persian PDF](persian/main_report.pdf). Each independent driver contributes a chapter. Project READMEs contain their own complete setup and tests, so firmware and usage do not depend on the report.

Both editions now follow a beginner sequence: wiring, first Keil build/download, live Watch and errors, two-function STM32 reuse, the runnable example, CubeMX pin changes, then protocol and selected implementation excerpts. The complete portable temperature function is explained after the first working example. The new vector cover presents the repository as a practical workbook and project report.

Start with DS18B20 steps 1–4 for the first hardware result, step 5 for reuse, then steps 8–10 for implementation. Live Watch needs Keil View → Periodic Window Update; volatile variables alone do not enable it.

## Build and edit

Install XeLaTeX/BibTeX (MiKTeX or TeX Live), including xepersian, IEEEtran, PGF/TikZ, needspace and fontawesome5. Install review dependencies with `python -m pip install pymupdf Pillow`. From the repository root:

```sh
python docs/tools/build_reports.py
python docs/tools/verify_reports.py
```

The build script obtains Vazirmatn Regular/Bold from the [↗ official upstream](https://github.com/rastikerdar/vazirmatn/tree/v33.003) if missing, then runs XeLaTeX → BibTeX → XeLaTeX twice. Font files remain ignored. In TeXstudio open the edition's `main_report.tex`, choose XeLaTeX and the same bibliography sequence. Each edition owns `config.tex`, `formatter.tex`, numbered `chapters/*.tex` and `references.bib`. Code listings read project sources from the full repository to avoid drifting documentation.

Fonts and colors are editable in `config.tex`. Persian uses xepersian with Vazirmatn; Latin text tries FreeSans, Noto Sans and DejaVu Sans, then the TeX-distributed Latin Modern fallback, and code uses Consolas if installed. Links use small Font Awesome icons at the surrounding font size. Exact author identity and six destinations appear on the cover, every page footer and the visible final contact list. Layout and sources are parallel in both languages.

The verifier renders **every page** under ignored `docs/tmp/`, checks author metadata and all six footer destinations, then creates contact sheets. Review those images after edits; link checks alone do not verify layout. The build fails on missing glyphs, unresolved citations and overfull boxes. Track only source and final `main_report.pdf`, not TeX auxiliary files, fonts or images.

## فارسی

نویسنده: **علیرضا خواجه‌وندی**.

گزارش کامل فقط در docs ریشه نگهداری می‌شود؛ هر پروژهٔ مستقل یک فصل متناظر دارد. README هر پروژه مستقل است. گزارش از اجرای اول شروع می‌شود؛ مرحله‌های ۱ تا ۴ برای اولین نتیجه، مرحلهٔ ۵ برای استفادهٔ دوباره و مراحل ۸ تا ۱۰ برای فهم پیاده‌سازی‌اند. callback و جزئیات بیت‌ها پس از نمونهٔ عملی توضیح داده می‌شوند. جلد جدید، مسیر سنسور تا کاربرد را با شکل برداری نشان می‌دهد و هدف آموزشی کل مخزن را معرفی می‌کند.

برای ساخت XeLaTeX و BibTeX و بسته‌های xepersian، IEEEtran، PGF/TikZ، needspace و fontawesome5 لازم‌اند. دو دستور بالا را از ریشه اجرا کنید. ابزار ساخت در صورت نبود فونت وزیرمتن آن را از upstream رسمی دانلود می‌کند؛ فونت وارد گیت نمی‌شود. در TeXstudio فایل main_report هر زبان را باز و ترتیب XeLaTeX، BibTeX و دو اجرای XeLaTeX را انتخاب کنید. رنگ و فونت در config و جلد در formatter قابل تغییر است. لیست کد از سورس پروژه خوانده می‌شود؛ ساخت گزارش به مخزن کامل نیاز دارد، نه اجرای firmware.

نام درست نویسنده و شش لینک با آیکون کوچک روی جلد، پاورقی تمام صفحات و بخش تماس نهایی هستند. verifier همهٔ صفحات را render و metadata و لینک‌های پاورقی را بررسی می‌کند. تصاویر docs/tmp را هم از نظر چیدمان ببینید. PDF نهایی هر دو زبان و سورس در گیت می‌مانند؛ دیتاشیت، فونت و خروجی بررسی و ساخت وارد گیت نمی‌شوند.
