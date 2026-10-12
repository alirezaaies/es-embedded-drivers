"""Structural checks; actual tool builds and CubeMX regeneration remain separate."""
from pathlib import Path
import xml.etree.ElementTree as ET
import re
root=Path(__file__).resolve().parents[1]
project=next((root/'MDK-ARM').glob('*.uvprojx'))
tree=ET.parse(project)
included=set()
for node in tree.findall('.//FilePath'):
    path=(project.parent/node.text.replace('\\','/')).resolve()
    assert path.is_relative_to(root), f'External source: {path}'
    assert path.is_file(), f'Missing source: {path}'
    included.add(path)
for pattern in ['Application/Src/*.c','Libraries/*/Src/*.c','Drivers/STM32F1xx_HAL_Driver/Src/*.c']:
    for path in root.glob(pattern): assert path.resolve() in included, f'Keil omitted: {path}'
assert (root/'MDK-ARM/startup_stm32f107xc.s').resolve() in included
assert (root/'Core/Src/system_stm32f1xx.c').resolve() in included
main=(root/'Core/Src/main.c').read_text(encoding="utf-8")
for marker, code in [('Includes','#include "app.h"'),('2','App_Init();'),('3','App_Process();')]:
    match=re.search(r'/\* USER CODE BEGIN '+marker+r' \*/(.*?)/\* USER CODE END '+marker+r' \*/',main,re.S)
    assert match and code in match[1], f'Unprotected App code: {code}'
ioc=next(root.glob('*.ioc')).read_text(encoding="utf-8")
assert 'ProjectManager.KeepUserCode=true' in ioc
assert 'RCC.PLLMUL=RCC_PLL_MUL9' in ioc
assert 'LENGTH = 256K' in (root/'STM32F107VC_FLASH.ld').read_text(encoding="utf-8")
if (root/'Libraries/DS18B20').exists():
    pin = re.search(r'^(P[A-G]\d+)\.GPIO_Label=ONE_WIRE_1$', ioc, re.M).group(1)
    for value in [f'{pin}.GPIO_ModeDefaultOutputPP=GPIO_MODE_OUTPUT_OD',
                  f'{pin}.GPIO_OutputLevel=GPIO_PIN_SET', f'{pin}.GPIO_PuPd=GPIO_NOPULL']:
        assert value in ioc
    header = (root/'Core/Inc/main.h').read_text(encoding="utf-8")
    assert re.search(r'#define\s+ONE_WIRE_1_Pin\s+GPIO_PIN_'+pin[2:]+r'\b', header)
    assert re.search(r'#define\s+ONE_WIRE_1_GPIO_Port\s+GPIO'+pin[1]+r'\b', header)
    assert 'ProjectManager.ProjectName=DS18B20' in ioc
    assert 'ProjectManager.ProjectFileName=DS18B20.ioc' in ioc
    for path in (root/'Libraries').rglob('*'):
        if path.suffix in ('.c', '.h'):
            assert not re.search(r'#include\s*[<"](?:stm32|main\.h)', path.read_text(encoding="utf-8")), path
print('PASS: local source paths, Keil membership, startup, USER CODE, IOC and Flash size')
