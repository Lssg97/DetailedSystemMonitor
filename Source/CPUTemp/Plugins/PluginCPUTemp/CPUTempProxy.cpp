#include <windows.h>
#include "CPUTempProxy.h"

CoreTempProxy::CoreTempProxy(void)
{
	gIsAMD = (IsCpuid()) ? IsAMD() : FALSE;
	
	RuningEn(gDriverPath);
	
	pdriver = new driver(OLS_DRIVER_ID, gDriverPath);

	_GetCoreCount();

	if (!gIsAMD)
	{
		_GetTjMax();
	}
}

CoreTempProxy::~CoreTempProxy(void)
{
	delete pdriver;
}

UINT CoreTempProxy::GetCoreLoad(int _index) const
{
	return this->m_pCoreTempData.uiLoad[_index];
}

//得到CPU理论最高温度（已实现）
UINT CoreTempProxy::GetTjMax() const
{
	return this->m_pCoreTempData.uiTjMax;
}

//得到CPU核心数（已实现）
UINT CoreTempProxy::GetCoreCount() const
{
	return this->m_pCoreTempData.uiCoreCnt;
}

//得到CPU数量
UINT CoreTempProxy::GetCPUCount() const
{
	return this->m_pCoreTempData.uiCPUCnt;
}

//得到单个CPU核心温度（已实现）
UINT CoreTempProxy::GetTemp(int _index) const
{
	return this->m_pCoreTempData.uiTemp[_index];
}

float CoreTempProxy::GetVID() const
{
	return this->m_pCoreTempData.fVID;
}

float CoreTempProxy::GetCPUSpeed() const
{
	return this->m_pCoreTempData.fCPUSpeed;
}

float CoreTempProxy::GetFSBSpeed() const
{
	return this->m_pCoreTempData.fFSBSpeed;
}

float CoreTempProxy::GetMultiplier() const
{
	return this->m_pCoreTempData.fMultiplier;
}

//得到CPU名称
LPCSTR CoreTempProxy::GetCPUName() const
{
	return this->m_pCoreTempData.sCPUName;
}

bool CoreTempProxy::IsFahrenheit() const
{
	return this->m_pCoreTempData.ucFahrenheit != 0;
}

bool CoreTempProxy::IsDistanceToTjMax() const
{
	return this->m_pCoreTempData.ucDeltaToTjMax != 0;
}

bool CoreTempProxy::IsTdpSupported() const
{
	return this->m_pCoreTempData.ucTdpSupported != 0;
}

bool CoreTempProxy::IsPowerSupported() const
{
	return this->m_pCoreTempData.ucPowerSupported != 0;
}

UINT CoreTempProxy::GetStructureVersion() const
{
	return this->m_pCoreTempData.uiStructVersion;
}

UINT CoreTempProxy::GetTdp(int _index) const
{
	return this->m_pCoreTempData.uiTdp[_index];
}

float CoreTempProxy::GetPower(int _index) const
{
	return this->m_pCoreTempData.fPower[_index];
}

float CoreTempProxy::GetMultiplier(int _index) const
{
	return this->m_pCoreTempData.fMultipliers[_index];
}



void CoreTempProxy::_GetCoreCount()
{
	SYSTEM_INFO info;
	GetSystemInfo(&info);
	m_pCoreTempData.uiCoreCnt=info.dwNumberOfProcessors;
}

void CoreTempProxy::_GetTjMax()
{
	DWORD eax = 0, edx = 0;
	pdriver->Rdmsr(0x1A2, &eax, &edx);
	m_pCoreTempData.uiTjMax = (eax & 0xff0000) >> 16;
}

void CoreTempProxy::_GetTemp(int _index)
{
	if (gIsAMD)
	{
		m_pCoreTempData.uiTemp[_index] = pdriver->GetTemp(_index);
	}
	else
	{
		DWORD eax = 0, edx = 0;
		pdriver->RdmsrTx(0x19C, &eax, &edx, _index);
		m_pCoreTempData.uiTemp[_index] = m_pCoreTempData.uiTjMax - ((eax & 0x7f0000) >> 16);
	}

}

LPCWSTR CoreTempProxy::GetErrorMessage()
{
	DWORD lastError;

	lastError = ::GetLastError();
	if ((lastError & UNKNOWN_EXCEPTION) > 0)
	{
		wcscpy_s(this->m_ErrorMessage, L"Unknown error occured while copying shared memory.");
	}
	else
	{
		::FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, lastError, 0, this->m_ErrorMessage, 99, NULL);
	}

	return this->m_ErrorMessage;
}