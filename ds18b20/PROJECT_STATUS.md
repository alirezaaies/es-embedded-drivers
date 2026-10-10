# Project status — 2026-10-10

## Report revision — 2026-10-10

User requested a practical function guide, removal of lengthy ending code
listings and more readable spacing. Both editions now omit the source appendix
and explain APIs with short examples within chapter 4: initialization, ROM,
blocking/split reads, arrays, replacement, configuration and error handling.
Body stretch changed from 1.25 to 1.5, paragraph gap from 6 to 8 pt, and code
size from scriptsize to small (10 pt) with stretch 1.15. Footer baseline remains
10.5 pt. These are current report settings; no global guideline change is imposed.
PDF page counts and validation below describe the earlier delivered version;
the updated report validation is recorded after the revision build.

Revision validation: English PDF 19 pages and Persian PDF 17 pages. Both rebuilt
with XeLaTeX/BibTeX; page contact sheets and the Persian body/function page were
visually reviewed. All six author links are present on every page and metadata
authors remain correct. Full-source appendix inputs and files were removed.
Only report/documentation files changed; firmware builds were not repeated.

Owner/author: **Alireza Khajehvandi / علیرضا خواجه‌وندی**.

## Objective and decisions

Create independent sensor/IC projects supporting CubeMX, Keil and PlatformIO.
First driver: DS18B20, tested MCU STM32F107VCT6, PB10. The copied folder was a
development reference rather than a product to preserve. Remove LCD, relays,
electrical measurement, Flash stores, unrelated libraries, logs and old build
artifacts. Keep a raw standalone STM32F107 foundation for later projects.

## Implemented

- Portable callback-based OneWire and DS18B20 addressed operations.
- Caller-sized ROM, scan, temperature and status arrays; no dynamic allocation.
- Cooperative broadcast conversion; one discovery ROM/read per service call.
- Configurable idle gap (250 ms default); conservative conversion wait (760 ms),
  timed after command completion, with 32-bit deadline wrap handling.
- Stable index mapping; one missing + one unknown ROM replaces automatically.
  Ambiguity, incomplete scans and discovery overflow inhibit remapping.
- CRC8, reserved/config bytes, signed/resolution decoding and range validation.
  Invalid values are NAN; inspect status before use.
- PB10 open-drain, 8 MHz HSE, 72 MHz core; DWT delay and prior PRIMASK restoration.
- Local vendor HAL/CMSIS used by both tools; full 256 KiB Flash, no reserved pages.
- CubeMX KeepUserCode and post-generation hook; Keil synchronization/check tools.
- Raw template under ../templates/stm32f107; no sibling dependency in either folder.
- English-first/Persian READMEs, official datasheet links and bilingual editable
  reports including implementation listings and author links.

## Validation evidence

- PlatformIO ST STM32 19.6.0, ARM GCC 7.2.1: DS18B20 builds for J-Link and ST-Link.
- Final example size: 9084 bytes Flash, 1332 bytes reported RAM. Raw template:
  4584 bytes Flash and 1152 bytes reported RAM (includes runtime reservations).
- Raw template PlatformIO build succeeds.
- Warning checks: -Wall -Wextra -Werror (unused callback parameters exempted).
- Cortex-M3 emulated C tests pass: signed/resolution decoding, ROM/scratchpad CRC,
  ordering, replacement, ambiguity, incomplete enumeration, overflow, absence,
  timeout, range rejection, wraparound and actual OneWire critical delay requests.
- Structural checks pass: local paths, startup and source membership, protected
  App hooks, IOC pin mode and Flash size. Keil synchronization is idempotent.
- Both reports compile using XeLaTeX/BibTeX with IEEEtran. Full page contact sheets
  and representative full pages inspected; all six link destinations on every
  page and metadata author verified. Downloaded fonts remain ignored by Git; final report PDFs are included.
  Final report lengths: English 33 pages, Persian 31 pages. No overfull boxes,
  missing characters or unresolved references in the final compilation logs.

## Cleanup record

Automatic tool review rejected the direct deletion command without a detailed
reason. The safe cleanup moved the original project, old datasheet PDFs and
temporary preparation scripts to `E:/akh/github/agfa/es-embedded-drivers-legacy-backup`,
outside this repository. They are absent from the delivered project trees.

## Work requiring the target or external GUI

Actual Keil compilation, actual CubeMX regeneration and hardware programming
have not been performed. Run the README regeneration commands, build Keil, then
follow the report's board acceptance procedure: ambient reading, multiple ROMs,
disconnect/replacement, ambiguous changes and waveforms under interrupt load.
The original supply/clock settings are assumptions from files, not measurements.
Timing figures are requested delays/nominal transaction durations, not measured
worst-case latency. No physical success is claimed.

## Intentional limits and continuation

External sensor power only. No parasite strong pull-up, Flash persistence,
automatic pairing of multiple missing sensors or DMA/timer transport. Address
mapping lives in RAM; load known ROMs or add application-owned persistence for
stable identity across reset. Hard interrupt deadlines below roughly 70 us plus
overhead require another transport. The addressed direct read needs a completed
conversion. A CRC-valid 85 C is accepted and cannot alone prove sensor freshness.

Before extending: read AGENTS.md and all agent-guidelines files. Keep IOC and both
build systems synchronized, use USER CODE boundaries, update both language
reports and this status file. PDFs of datasheets must not enter the project.
'''

## Report publication policy ? 2026-10-10

User explicitly requested report PDFs on GitHub. Root/project/template ignore
rules now allow docs/english/main_report.pdf and docs/persian/main_report.pdf.
AGENTS and local guidelines require retaining both final PDFs with LaTeX sources
and rebuilding them after content edits. Datasheet PDFs, fonts and temporary
render/build files remain excluded. This supersedes the initial PDF exclusion.
