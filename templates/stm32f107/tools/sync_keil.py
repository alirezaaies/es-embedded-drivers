from pathlib import Path
import xml.etree.ElementTree as ET
root = Path(__file__).resolve().parents[1]
project = next((root/'MDK-ARM').glob('*.uvprojx'))
tree = ET.parse(project)
target = tree.find('.//Target')
groups = target.find('Groups')
for group in list(groups):
    name = group.findtext('GroupName', '')
    if name not in ('Application/User/Core', 'Drivers/CMSIS'):
        groups.remove(group)
for name, paths in [('Application', sorted((root/'Application/Src').glob('*.c'))),
                    ('Drivers/STM32F1xx_HAL_Driver', sorted((root/'Drivers/STM32F1xx_HAL_Driver/Src').glob('*.c'))),
                    ('Startup', sorted((root/'MDK-ARM').glob('startup_*.s'))),
                    ('Portable drivers', sorted((root/'Libraries').glob('*/Src/*.c')))]:
    group = ET.SubElement(groups, 'Group')
    ET.SubElement(group, 'GroupName').text = name
    files = ET.SubElement(group, 'Files')
    for path in paths:
        f = ET.SubElement(files, 'File')
        ET.SubElement(f, 'FileName').text = path.name
        ET.SubElement(f, 'FileType').text = '2' if path.suffix == '.s' else '1'
        ET.SubElement(f, 'FilePath').text = '..\\' + str(path.relative_to(root)).replace('/', '\\')
inc = target.find('.//Cads/VariousControls/IncludePath')
inc.text = ';'.join(['../Core/Inc','../Drivers/STM32F1xx_HAL_Driver/Inc',
                    '../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy',
                    '../Drivers/CMSIS/Device/ST/STM32F1xx/Include','../Drivers/CMSIS/Include',
                    '../Application/Inc'] + ['../'+str(x.relative_to(root)).replace('\\','/') for x in sorted((root/'Libraries').glob('*/Inc'))])
for e in target.findall('.//IROM1/Size') + target.findall('.//OCR_RVCT4/Size'):
    e.text = '0x40000'
target.find('.//TargetCommonOption/OutputDirectory').text = 'build\\'
name = project.stem
target.find('TargetName').text = name
target.find('.//TargetCommonOption/OutputName').text = name
cpu = target.find('.//TargetCommonOption/Cpu')
cpu.text = cpu.text.replace('CLOCK(25000000)', 'CLOCK(72000000)')
for info in tree.findall('.//targetInfo'):
    info.set('name', name)
for e in target.findall('.//RunUserProg2'): e.text = '0'
ET.indent(tree, space='  ')
tree.write(project, encoding='utf-8', xml_declaration=True)
print('Synchronized', project.name)
