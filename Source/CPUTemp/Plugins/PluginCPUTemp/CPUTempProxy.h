#pragma once
#include "driver.h"
#include "running_environment.h"
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


	DWORD GetDllError() const { return GetLastError(); }
	LPCWSTR GetErrorMessage();

	void _GetCoreCount();
	void _GetTjMax();
	void _GetTemp(int _index);
private:
	struct core_temp_data
	{
		// Original structure (CoreTempSharedData)
		unsigned int	uiLoad[256];
		unsigned int	uiTjMax;
		unsigned int	uiCoreCnt;
		unsigned int	uiCPUCnt;
		unsigned int	uiTemp[256];
		float			fVID;
		float			fCPUSpeed;
		float			fFSBSpeed;
		float			fMultiplier;
		char			sCPUName[100];
		unsigned char	ucFahrenheit;
		unsigned char	ucDeltaToTjMax;
		// uiStructVersion = 2
		unsigned char	ucTdpSupported;
		unsigned char	ucPowerSupported;
		unsigned int	uiStructVersion;
		unsigned int	uiTdp[128];
		float			fPower[128];
		float			fMultipliers[256];
	} m_pCoreTempData;

	WCHAR m_ErrorMessage[100];
	INT gIsAMD = FALSE;
	TCHAR gDriverPath[MAX_PATH];
	driver* pdriver;
};
