#pragma once

#include "DriverFunc.h"



#define PciBusDevFunc(bus,dev,func)	(((bus&0xFF)<<8) | ((dev&0x1F)<<3) | (func&7))
#define PciGetBus(address)			((address>>8) & 0xFF)
#define PciGetDev(address)			((address>>3) & 0x1F)
#define PciGetFunc(address)			(address&7)

#define DeviceSlot(uDevice, uFunction) ((((uDevice)&0x1f)<<3)|((uFunction)&0x07))
#define GetDevice(uBus,uSlot,uAddress) (0x80000000L |((uBus&0xff)<<16)|(uSlot<<8)|(uAddress&~3))

BOOL get_bus_dev(UINT devieid, INT* BUS, INT* DEV);
DWORD get_temp();
DWORD GetAMDTemp(DWORD_PTR threadAffinityMask);
int _GetTjMax();
int _GetTemp(int _index);
