# Standalone projects and the central report

1. Read all agent-guidelines and project AGENTS before editing.
2. Copy the raw template into a new project. Rename IOC/project/tool outputs; keep local vendor/startup/linker files. Never depend on the template or a sibling during a firmware build.
3. Start with the smallest useful API. Portable Libraries contain no MCU headers. Application owns hardware callbacks and a runnable board acceptance test. Preserve CubeMX hooks inside USER CODE and use generated pin labels.
4. Keep an English-first/Persian README in each project with wiring, commands, API, error handling, regeneration, official datasheet revision/link and honest validation. It must remain useful when the project is copied alone.
5. Root docs/ is the only complete report: one English and Persian main_report.pdf, editable sources, separate numbered chapter files, IEEE references and xepersian. Add each new driver as a chapter in both main_report.tex files and update the catalog. Per-project docs contain only a pointer; no duplicate PDF reports.
6. Teach in beginner order: purpose, wiring, first build/download, visible output and debugger refresh, errors, minimal reuse API, runnable example and regeneration, then protocol and implementation reasoning. Explain pointers/callbacks before using them in advanced sections. Give an expected result after practical steps and avoid beginning with large source listings. Rebuild both reports with python docs/tools/build_reports.py and verify/render them after every content change.
7. Preserve exact six author URLs on cover, every footer and final contact page. Use font-size Font Awesome icons and readable labels; brand colors may be used subtly. External report links use a small external-link icon. Do not fabricate professional titles.
8. Datasheet PDFs, local fonts, rendered images, build products and probe logs are ignored. Track final central report PDFs. Update local PROJECT_STATUS.md with actual tool versions/results and unperformed board tests; never present simulation as hardware validation.
9. After CubeMX generation run local tools/sync_keil.py, tools/check_project.py, portable tests and both toolchain builds. Test regeneration in a disposable copy when changing the workflow. Keep evidence of what was preserved.
10. Distinguish initial values, fresh measurements and stale debugger displays. Covers identify the whole repository, its learning goals and reuse path. Prefer editable vector artwork and preserve all author links.

## فارسی

هر پروژه مستقل است و README باید برای کپی همان پوشه کافی باشد. گزارش فقط در docs ریشه و برای هر پروژه یک فصل متناظر در دو زبان است. آموزش از اتصال و اجرای اول و مشاهدهٔ نتیجه شروع شود؛ refresh دیباگر، کنترل خطا و API ساده پیش از callback و جزئیات پیاده‌سازی بیایند. برای هر مرحله نتیجۀ قابل بررسی نوشته شود. جلد هدف آموزشی کل مخزن را نشان دهد. هویت نویسنده و شش لینک آیکون‌دار در جلد، همهٔ پاورقی‌ها و تماس حفظ شوند. هر دو PDF بعد از تغییر ساخته و بررسی شوند؛ ساخت و شبیه‌سازی و مشاهدهٔ واقعی برد جدا ثبت شوند.
