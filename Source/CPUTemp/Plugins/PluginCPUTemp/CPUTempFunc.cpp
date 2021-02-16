#include "CPUTempFunc.h"

DWORD GetCoreCount()
{
	SYSTEM_INFO info;
	GetSystemInfo(&info);
	return info.dwNumberOfProcessors;
}

UINT GetTjMax()
{
	return _GetTjMax();
}

UINT GetTemp(BYTE index)
{
	return _GetTemp(index);
}

UINT GetHighestTemp()
{
	int MaxTemp = -255;
	DWORD coreCount = GetCoreCount();

	for (UINT i = 1; i <= coreCount; ++i)
	{
		const int Temp = GetTemp(i);
		if (MaxTemp < Temp) MaxTemp = Temp;
	}
	return MaxTemp;
}