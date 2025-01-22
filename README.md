# MM32

## Overview

mm32 is a standalone set of drivers for peripherals present in mm32's
SoCs. It originated as an extract from the mm32 SDK.
The intention was to provide drivers that can be used in various environments
without the necessity to integrate other parts of the SDK into them.
This package contains definitions of register structures and bitfields 
for all supported SoCs, as well as startup and initialization files for them.

## Supported SoCs

* mm32f5260 Series

## Directories

```
.
├── CMSIS
│   └── Core
├── HAL_Lib
│   ├── Inc
│   └── Src
├── Include
├── SConscript
├── Soc
│   └── mm32f5260
└── System
```

