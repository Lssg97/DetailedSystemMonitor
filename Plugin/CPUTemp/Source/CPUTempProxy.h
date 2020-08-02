#pragma once
#include "SharedMem.h"
#include "driver.h"
#include "../../Library/Export.h"

#define UNKNOWN_EXCEPTION 0x20000000

class CoreTempProxy
{
public:
	CoreTempProxy(void);
	virtual ~CoreTempProxy(void);
	
	UINT GetCoreLoad(int Index) const;
	UINT GetTjMax() const;
	UINT GetCoreCount() const;
	UINT GetCPUCount() const;
	UINT GetTemp(int Index) const;
	float GetVID() const;
	float GetCPUSpeed() const;
	float GetFSBSpeed() const;
	float GetMultiplier() const;
	LPCSTR GetCPUName() const;
	bool IsFahrenheit() const;
	bool IsDistanceToTjMax() const;
	bool IsTdpSupported() const;
	bool IsPowerSupported() const;
	UINT GetStructureVersion() const;
	UINT GetTdp(int Index) const;
	float GetPower(int Index) const;
	float GetMultiplier(int Index) const;

	const CoreTempSharedDataEx &GetDataStruct() const;

	bool GetData();
	DWORD GetDllError() const { return GetLastError(); }
	LPCWSTR GetErrorMessage();

	void _GetCoreCount();
	void _GetTjMax();
	void _GetTemp(int _index);
private:
	CSharedMemClient m_SharedMem;
	CoreTempSharedDataEx m_pCoreTempData;
	WCHAR m_ErrorMessage[100];

	TCHAR gDriverPath[MAX_PATH];
	driver* pdriver;
};
