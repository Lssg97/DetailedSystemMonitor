/* Copyright (C) 2011 Rainmeter Project Developers
 *
 * This Source Code Form is subject to the terms of the GNU General Public
 * License; either version 2 of the License, or (at your option) any later
 * version. If a copy of the GPL was not distributed with this file, You can
 * obtain one at <https://www.gnu.org/licenses/gpl-2.0.html>. */

#include <windows.h>
#include <wchar.h>
#include "CPUTempFunc.h"
#include "../../Library/Export.h"	// Rainmeter's exported functions

enum eMeasureType
{
	MeasureTemperature,
	MeasureMaxTemperature,
	MeasureTjMax,
	MeasureLoad,
	MeasureVid,
	MeasureCpuSpeed,
	MeasureBusSpeed,
	MeasureBusMultiplier,
	MeasureCpuName,
	MeasureCoreSpeed,
	MeasureCoreBusMultiplier,
	MeasureTdp,
	MeasurePower,
};

struct MeasureData
{
	eMeasureType type;
	int index;

	void* rm;

	MeasureData() :
		type(MeasureTemperature),
		index(0),
		rm(nullptr) {}
};

eMeasureType convertStringToMeasureType(LPCWSTR i_String, void* rm);
bool areStringsEqual(LPCWSTR i_String1, LPCWSTR i_Strting2);

PLUGIN_EXPORT void Initialize(void** data, void* rm)
{
	MeasureData* measure = new MeasureData;
	*data = measure;

	measure->rm = rm;
}

PLUGIN_EXPORT void Reload(void* data, void* rm, double* maxValue)
{
	UNREFERENCED_PARAMETER(maxValue);
	MeasureData* measure = (MeasureData*)data;

	LPCWSTR value = RmReadString(rm, L"CPUTempType", L"Temperature");
	measure->type = convertStringToMeasureType(value, rm);
	measure->index = RmReadInt(rm, L"CPUTempIndex", 1);
}

PLUGIN_EXPORT double Update(void* data)
{
	
	MeasureData* measure = (MeasureData*)data;
	double result = 0;

	switch (measure->type)
	{
	case MeasureTemperature:
		result = GetTemp(measure->index);
		break;

	case MeasureMaxTemperature:
		result = GetHighestTemp();
		break;

	case MeasureTjMax:
		result = GetTjMax();
		break;

	case MeasureLoad:
		break;

	case MeasureVid:
		break;

	case MeasureCpuSpeed:
		break;

	case MeasureBusSpeed:
		break;

	case MeasureBusMultiplier:
		break;

	case MeasureCoreSpeed:
		break;

	case MeasureCoreBusMultiplier:
		break;

	case MeasureTdp:
		break;

	case MeasurePower:
		break;
	}

	return result;
}

PLUGIN_EXPORT LPCWSTR GetString(void* data)
{
	MeasureData* measure = (MeasureData*)data;
	static WCHAR buffer[128];

	switch (measure->type)
	{
	case MeasureVid:
		_snwprintf_s(buffer, _TRUNCATE, L"%.4f", 0.0);
		break;

	case MeasureCpuName:
		_snwprintf_s(buffer, _TRUNCATE, L"%S", "0");
		break;

	default:
		return nullptr;
	}

	return buffer;
}

PLUGIN_EXPORT void Finalize(void* data)
{
	MeasureData* measure = (MeasureData*)data;
	delete measure;
}

bool areStringsEqual(LPCWSTR i_String1, LPCWSTR i_Strting2)
{
	return _wcsicmp(i_String1, i_Strting2) == 0;
}

eMeasureType convertStringToMeasureType(LPCWSTR i_String, void* rm)
{
	eMeasureType result;

	if (areStringsEqual(i_String, L"Temperature"))
	{
		result = MeasureTemperature;
	}
	else if (areStringsEqual(i_String, L"MaxTemperature"))
	{
		result = MeasureMaxTemperature;
	}
	else if (areStringsEqual(i_String, L"TjMax"))
	{
		result = MeasureTjMax;
	}
	else if (areStringsEqual(i_String, L"Load"))
	{
		result = MeasureLoad;
	}
	else if (areStringsEqual(i_String, L"Vid"))
	{
		result = MeasureVid;
	}
	else if (areStringsEqual(i_String, L"CpuSpeed"))
	{
		result = MeasureCpuSpeed;
	}
	else if (areStringsEqual(i_String, L"BusSpeed"))
	{
		result = MeasureBusSpeed;
	}
	else if (areStringsEqual(i_String, L"BusMultiplier"))
	{
		result = MeasureBusMultiplier;
	}
	else if (areStringsEqual(i_String, L"CpuName"))
	{
		result = MeasureCpuName;
	}
	else if (areStringsEqual(i_String, L"CoreSpeed"))
	{
		result = MeasureCoreSpeed;
	}
	else if (areStringsEqual(i_String, L"CoreBusMultiplier"))
	{
		result = MeasureCoreBusMultiplier;
	}
	else if (areStringsEqual(i_String, L"Tdp"))
	{
		result = MeasureTdp;
	}
	else if (areStringsEqual(i_String, L"Power"))
	{
		result = MeasurePower;
	}
	else
	{
		result = MeasureTemperature;
		RmLogF(rm, LOG_WARNING, L"ÎÞÐ§µÄCPUTempType: %s", i_String);
	}

	return result;
}