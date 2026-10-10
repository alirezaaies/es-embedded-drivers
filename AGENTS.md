# Repository working rules

Read every file in `agent-guidelines` before changing a project. Each project
must be self-contained: local CubeMX IOC, Keil project, PlatformIO, vendor files,
application adapters, portable drivers, status and documentation. Keep portable
libraries free of MCU headers. Keep generated Core edits inside USER CODE blocks;
hardware setup must agree with IOC. Run the project's tools/sync_keil.py after
CubeMX generation, then its structural checks and appropriate builds/tests.

READMEs start in English, then Persian. Reports follow the supplied bilingual
LaTeX structure, xepersian, IEEE bibliography and exact author branding. Preserve
all six author links on cover, every page footer and final contact section.
Track editable sources; do not track PDF, font, rendering or build artifacts.
Datasheets are official links only. Update PROJECT_STATUS.md with real validation
and unfinished hardware work; never equate simulated tests with board validation.

Do not use the raw template as a shared build dependency. Copy it into a new
standalone project. No agent delegation is required by these guidelines.
