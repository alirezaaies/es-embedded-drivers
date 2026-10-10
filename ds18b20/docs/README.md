# Editable project reports

Both language editions use the same A4 theme, margins, accent colour, code
formatting and author/contact data. Each has main_report.tex, config.tex,
formatter.tex, chapters and an IEEE-style BibTeX bibliography. Open main_report.tex
in TeXstudio and choose XeLaTeX. Build from the edition's directory:

```text
xelatex -interaction=nonstopmode -halt-on-error main_report.tex
bibtex main_report
xelatex -interaction=nonstopmode -halt-on-error main_report.tex
xelatex -interaction=nonstopmode -halt-on-error main_report.tex
```

MiKTeX/TeX Live packages: xepersian, fontspec, geometry, fancyhdr, listings,
titlesec, hyperref, IEEEtran. Persian uses the free Vazirmatn font when the local
docs/fonts/Vazirmatn-Regular.ttf is present, then IRANSansX, IRANSans, B Nazanin.
Download Vazirmatn from https://github.com/rastikerdar/vazirmatn (SIL OFL).
Preferred FreeSans, Noto Sans and DejaVu Sans were checked but unavailable; English uses Latin Modern.
Change font selection and author links centrally in config.tex.
The function guide explains inputs, outputs, timing and error handling with short
examples beside the explanation. Full source listings are intentionally omitted;
read the implementation files in Application and Libraries when needed.

Current typography settings, identical in both language editions:

- Body: 11 pt; `\setstretch{1.5}` (1.5 times the font's normal baseline).
- Paragraph gap: 8 pt (`\parskip`), separate from line spacing.
- Code: `\small` (10 pt in this document class), stretch 1.15, wrapped lines.
- Footer: 8.5 pt with a fixed 10.5 pt baseline to keep contact links compact.

Edit these values in config.tex. The body multiplier is a layout choice for
this report, not a universal publishing standard. With this 11 pt article class,
the nominal body baseline is approximately 20.4 pt (7.2 mm).
PDFs and fonts remain local and are excluded from Git; report sources are tracked.
Read ../agent-guidelines before extending either edition.

## گزارش‌های قابل‌ویرایش

هر دو زبان قالب، رنگ و ساختار یکسان دارند. فایل main_report را در TeXstudio
باز و موتور XeLaTeX را انتخاب کنید. دستورهای بالا را از پوشهٔ همان زبان اجرا
کنید. فونت رایگان وزیر از پروژهٔ رسمی بالا قابل دریافت است؛ فونت و PDF محلی
هستند. تنظیمات فونت، نام نویسنده و لینک‌ها در config.tex متمرکز است. پیوست کامل
کد حذف شده و مثال‌های کوتاه همراه توضیح ورودی، خروجی، زمان استفاده و خطاها در
بخش توابع قرار دارند. برای پیاده‌سازی کامل به فایل‌های کتابخانه مراجعه کنید.

متن اصلی ۱۱ پوینت و ضریب فاصلهٔ خطوط ۱٫۵ دارد؛ یعنی فاصلهٔ خط پایه ۱٫۵ برابر
حالت عادی فونت، در این قالب تقریباً ۲۰٫۴ پوینت یا ۷٫۲ میلی‌متر. فاصلهٔ پاراگراف‌ها
۸ پوینت است و از فاصلهٔ خطوط جداست. کدها با اندازهٔ ۱۰ پوینت و ضریب ۱٫۱۵
نمایش داده می‌شوند. پاورقی اندازهٔ ۸٫۵ و خط پایهٔ ۱۰٫۵ پوینت دارد. این تنظیم
انتخاب فعلی گزارش است و استاندارد اجباری عمومی یا قاعدهٔ قطعی همهٔ پروژه‌ها نیست.
