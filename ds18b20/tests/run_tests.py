"""Execute the actual portable C library on an emulated Cortex-M3.
Requires PlatformIO ARM GCC and pip install unicorn.
"""
from pathlib import Path
import subprocess, tempfile, os
from unicorn import Uc, UC_ARCH_ARM, UC_MODE_THUMB, UC_HOOK_CODE
from unicorn.arm_const import UC_ARM_REG_SP, UC_ARM_REG_LR, UC_ARM_REG_R0, UC_ARM_REG_PC
root = Path(__file__).resolve().parents[1]
packages = Path(os.environ.get('PLATFORMIO_CORE_DIR', str(Path.home()/'.platformio')))/'packages'
bin_dir = packages/'toolchain-gccarmnoneeabi/bin'
suffix = '.exe' if os.name == 'nt' else ''
def run_suite(test_source, library_sources, extra_flags=()):
  with tempfile.TemporaryDirectory() as temp:
    temp = Path(temp)
    elf, binary = temp/'test.elf', temp/'test.bin'
    sources = [root/test_source] + library_sources
    cmd = [str(bin_dir/('arm-none-eabi-gcc'+suffix)), '-mcpu=cortex-m3','-mthumb',
           '-O1','-g','-Wall','-Wextra','-Werror','-nostartfiles',
           '-Wl,-Ttext=0x10000,-Tdata=0x20000000,-e,main']
    cmd += ['-I'+str(root/'Libraries/DS18B20/Inc'), '-I'+str(root/'Libraries/OneWire/Inc')]
    cmd += list(extra_flags)
    subprocess.run(cmd + [str(s) for s in sources] + ['-o',str(elf)],check=True)
    objcopy = str(bin_dir/('arm-none-eabi-objcopy'+suffix))
    subprocess.run([objcopy, '-O','binary','-j','.text','-j','.rodata',str(elf),str(binary)],check=True)
    initialized = temp/'data.bin'
    subprocess.run([objcopy, '-O','binary','-j','.data',str(elf),str(initialized)],check=True)
    symbols = subprocess.check_output([str(bin_dir/('arm-none-eabi-nm'+suffix)), str(elf)],text=True)
    entry = next(int(line.split()[0],16) for line in symbols.splitlines() if line.endswith(' T main'))
    cpu = Uc(UC_ARCH_ARM,UC_MODE_THUMB)
    data=binary.read_bytes()
    cpu.mem_map(0x10000, (len(data)+4095)&~4095)
    cpu.mem_write(0x10000,data)
    cpu.mem_map(0x20000000,0x20000)
    cpu.mem_write(0x20000000, initialized.read_bytes())
    cpu.reg_write(UC_ARM_REG_SP,0x2001FFF0)
    cpu.reg_write(UC_ARM_REG_LR,0x18001)
    cpu.emu_start(entry|1,0x18000,count=20000000)
    assert cpu.reg_read(UC_ARM_REG_PC) == 0x18000, 'C test did not return before instruction limit'
    result=cpu.reg_read(UC_ARM_REG_R0)
    if result: raise AssertionError(f'C test failed at line {result}')
    print('PASS:', test_source)

run_suite('tests/test_driver.c', [root/'Libraries/DS18B20/Src/ds18b20.c'])
run_suite('tests/test_one_wire.c', [root/'Libraries/OneWire/Src/one_wire.c'])
run_suite('tests/test_app.c', [root/'Application/Src/app.c'],
          ['-DSTM32F107xC', '-DUSE_HAL_DRIVER'] +
          ['-I'+str(root/path) for path in ['Application/Inc', 'Core/Inc',
           'Drivers/STM32F1xx_HAL_Driver/Inc', 'Drivers/CMSIS/Include',
           'Drivers/CMSIS/Device/ST/STM32F1xx/Include']])
