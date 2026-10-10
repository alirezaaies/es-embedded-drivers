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
main=(root/'Core/Src/main.c').read_text()
for marker, code in [('Includes','#include "app.h"'),('2','App_Init();'),('3','App_Process();')]:
    match=re.search(r'/\* USER CODE BEGIN '+marker+r' \*/(.*?)/\* USER CODE END '+marker+r' \*/',main,re.S)
    assert match and code in match[1], f'Unprotected App code: {code}'
ioc=next(root.glob('*.ioc')).read_text()
assert 'ProjectManager.KeepUserCode=true' in ioc
assert 'RCC.PLLMUL=RCC_PLL_MUL9' in ioc
assert 'LENGTH = 256K' in (root/'STM32F107VC_FLASH.ld').read_text()
if (root/'Libraries/DS18B20').exists():
    for value in ['PB10.GPIO_ModeDefaultOutputPP=GPIO_MODE_OUTPUT_OD',
                  'PB10.GPIO_OutputLevel=GPIO_PIN_SET', 'PB10.GPIO_PuPd=GPIO_NOPULL']:
        assert value in ioc
    assert '#define ONE_WIRE_1_Pin GPIO_PIN_10' in (root/'Core/Inc/main.h').read_text()
print('PASS: local source paths, Keil membership, startup, USER CODE, IOC and Flash size')
