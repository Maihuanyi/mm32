# for mm32 soc module compiling
from building import *
import rtconfig

cwd = GetCurrentDir()
path = []

src = Glob('./HAL_Lib/Src/*.c')

if GetDepend('SOC_MM32F526x') == True:
    src += ['./System/system_mm32f526x.c']

# include
path += [cwd + '/CMSIS/Core']
path += [cwd + '/Soc/mm32f5260']
path += [cwd + '/Include']
path += [cwd + '/HAL_Lib/Inc']

CPPPATH = path

if rtconfig.PLATFORM in ['gcc']:
    src += [./System/startup_mm32f5260_gcc.s']
elif rtconfig.PLATFORM in ['armcc', 'armclang']:
    src += [./System/startup_mm32f5260_keil.c']
elif rtconfig.PLATFORM in ['iccarm']:
    src += [./System/startup_mm32f5260_iar.s']

group = DefineGroup('Libraries', src, depend = [''], CPPPATH = CPPPATH)

Return('group')
