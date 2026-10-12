"""Enable live Watch refresh and add global measurement expressions.

Run with Keil closed. The ignored .uvoptx stores local probe/session choices;
this script preserves them, as well as existing watches and breakpoints.
For an already open session use View > Periodic Window Update in Keil.
"""
from pathlib import Path
import shutil
import xml.etree.ElementTree as ET

root = Path(__file__).resolve().parents[1]
options = root / 'MDK-ARM/DS18B20.uvoptx'
if not options.exists():
    raise SystemExit('Open/save the project in Keil once, close it, then run this tool. '
                     'Or enable View > Periodic Window Update in the debug session.')
tree = ET.parse(options)
targets = tree.findall('.//TargetOption')
if not targets:
    raise SystemExit('No TargetOption in Keil options; file was not changed.')
expressions = ['g_temperature_c', 'g_temperature_status', 'g_temperature_valid',
               'g_measurement_busy', 'g_sample_cycles', 'g_successful_samples',
               'g_last_success_ms']
changed = False
for target in targets:
    flags = target.find('DebugFlag')
    if flags is None:
        flags = ET.SubElement(target, 'DebugFlag')
        changed = True
    periodic = flags.find('periodic')
    if periodic is None:
        periodic = ET.SubElement(flags, 'periodic')
    if periodic.text != '1':
        periodic.text = '1'
        changed = True
    watches = target.find('WatchWindow1')
    if watches is None:
        watches = ET.SubElement(target, 'WatchWindow1')
    existing = {entry.findtext('ItemText') for entry in watches.findall('Ww')}
    count = max((int(e.findtext('count', '-1')) for e in watches.findall('Ww')), default=-1) + 1
    for expression in expressions:
        if expression in existing:
            continue
        changed = True
        entry = ET.SubElement(watches, 'Ww')
        ET.SubElement(entry, 'count').text = str(count)
        ET.SubElement(entry, 'WinNumber').text = '1'
        ET.SubElement(entry, 'ItemText').text = expression
        count += 1
if not changed:
    print('Periodic Window Update and the global Watch expressions are already configured.')
    print('For an open session, check View > Periodic Window Update in its loaded settings.')
    raise SystemExit(0)
shutil.copy2(options, options.with_suffix('.uvoptx.bak'))
ET.indent(tree, space='  ')
tree.write(options, encoding='utf-8', xml_declaration=True)
print('Enabled Periodic Window Update; added global Watch expressions.')
print('An open Keil session keeps its in-memory options: enable View > Periodic Window '
      'Update there too. Run this tool again AFTER closing Keil if it saves old options.')
