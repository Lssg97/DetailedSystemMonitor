#include <Windows.h>
#include "../API/RainmeterAPI.h"
#include "../nvapi-337/nvapi.h"
#ifdef _WIN64
#pragma comment(lib,"../nvapi-337/amd64/nvapi64.lib")
#else
#pragma comment(lib,"../nvapi-337/x86/nvapi.lib")
#endif

#define N_MAX_GPUS 3
//#define N_MAX_PHYSICAL_GPUS NVAPI_MAX_PHYSICAL_GPUS

static BOOL thread = false;
static HANDLE hEvent = NULL;
static HANDLE hThread = NULL;
static BYTE bUpdate[N_MAX_GPUS] = { 0 };
static DWORD WINAPI ThreadProc(LPVOID lpParameter);

enum MeasureType
{
	GPU_CurTemp,
	GPU_CurMaxTemp,
	GPU_MaxTemp,
	GPU_Usage,
	GPU_MaxUsage
};

class Measure
{
public:

	MeasureType type;

	BOOL advance;
	DWORD nGPUToGet;
	static DWORD nCurTemp[N_MAX_GPUS];
	static DWORD nCurMaxTemp[N_MAX_GPUS];
	static DWORD nMaxTemp[N_MAX_GPUS];
	static DWORD nUsage[N_MAX_GPUS];
	static DWORD nMaxUsage[N_MAX_GPUS];
	static int iNvGPUInit;
	static NvU32 nNvGPU;

	Measure() : nGPUToGet(0), advance(false) {}
	~Measure(){}

	void InitializeGPU(void* measure)
	{
		if (!iNvGPUInit)
		{
			if (NvAPI_Initialize() != NVAPI_OK) return;

			if (NvAPI_EnumPhysicalGPUs(hNvGPU, &nNvGPU) != NVAPI_OK) return;

			iNvGPUInit = 1;

			//NvAPI_Status ret = NVAPI_OK;
			//NV_GPU_TYPE type;
			//ret = NvAPI_GPU_GetGPUType(hNvGPU[0], &type);		

			if (measure)
			{
				thread = true;
				//hEvent = CreateEvent(NULL, TRUE, TRUE, NULL);
				hEvent = CreateEvent(NULL, FALSE, TRUE, NULL);
				hThread = CreateThread(NULL, 0, ThreadProc, (LPVOID)measure, 0, NULL);
			}	
		}
		else iNvGPUInit++;

		if (nGPUToGet+1 > nNvGPU) 
		{ 
			nGPUToGet = 0; 
			RmLog(LOG_ERROR, L"NvidiaGPU.dll: Invalid GPU");
		}
	}

	void FinalizeGPU()
	{
		if (hThread)
		{
			thread = false;
			SetEvent(hEvent);
			WaitForSingleObject(hThread, 500);
			//WaitForSingleObject(hThread, INFINITE);
			CloseHandle(hEvent);
			hEvent = NULL;
			CloseHandle(hThread);
			hThread = NULL;
		}		
		NvAPI_Unload();
	}

	void GetGPUTemperature(DWORD nToGet)
	{
		if (iNvGPUInit == 0 || hNvGPU[nToGet] == NULL) {
			return;
		}
		if (advance) {
			GetGPUTemperatureAdvance(nToGet);
			return;
		}

		NV_GPU_THERMAL_SETTINGS thermal;
		thermal.version = NV_GPU_THERMAL_SETTINGS_VER;
		if (NvAPI_GPU_GetThermalSettings(hNvGPU[nToGet], 0, &thermal) != NVAPI_OK) {
			advance = true;
			GetGPUTemperatureAdvance(nToGet);
			return;
		}
		else
		{
			nCurTemp[nToGet] = thermal.sensor[0].currentTemp;
			if (nCurTemp[nToGet] > nCurMaxTemp[nToGet]) nCurMaxTemp[nToGet] = nCurTemp[nToGet];
			nMaxTemp[nToGet] = thermal.sensor[0].defaultMaxTemp;
		}
	}

	void GetGPUUsage(DWORD nToGet)
	{
		if (iNvGPUInit == 0 || hNvGPU[nToGet] == NULL) {
			nUsage[nToGet] = 101;
			return;
		}

		NV_GPU_DYNAMIC_PSTATES_INFO_EX infoEx;
		infoEx.version = NV_GPU_DYNAMIC_PSTATES_INFO_EX_VER;
		if (NvAPI_GPU_GetDynamicPstatesInfoEx(hNvGPU[nToGet], &infoEx) != NVAPI_OK || infoEx.utilization[0].bIsPresent == 0) {
			nUsage[nToGet] = 101;
			return;
		}
		nUsage[nToGet] = infoEx.utilization[0].percentage;
		if (nUsage[nToGet] > nMaxUsage[nToGet]) nMaxUsage[nToGet] = nUsage[nToGet];
		//return static_cast<int>(infoEx.utilization[0].percentage);
	}

	void GetGPUName(void)
	{
		NvAPI_Status status;
		NvAPI_ShortString sName;

		WCHAR info[100];
		for (DWORD n = 0; n < nNvGPU; n++)
		{
			status = NvAPI_GPU_GetFullName(hNvGPU[n], sName);
			if (status == NVAPI_OK)
			{
				MultiByteToWideChar(CP_ACP, 0, sName, -1, info+10, 100);
				wsprintf(info, L"GPU%d : %s", n, info+10);
				RmLog(LOG_NOTICE, info);
			}
		}
	}

private:

	void InitThermal(NV_GPU_THERMAL_SETTINGS& thermal, NvU32 Version)
	{
		ZeroMemory(&thermal, sizeof(NV_GPU_THERMAL_SETTINGS));
		thermal.version = Version;
		thermal.count = NVAPI_MAX_THERMAL_SENSORS_PER_GPU;
	}

	void GetGPUTemperatureAdvance(DWORD nToGet)
	{
		NvAPI_Status ret = NVAPI_OK;
		NV_GPU_THERMAL_SETTINGS thermal;
		InitThermal(thermal, NV_GPU_THERMAL_SETTINGS_VER);
		ret = NvAPI_GPU_GetThermalSettings(hNvGPU[nToGet], 0, &thermal);
		if (ret == NVAPI_INCOMPATIBLE_STRUCT_VERSION)
		{
			InitThermal(thermal, NV_GPU_THERMAL_SETTINGS_VER_1);
			ret = NvAPI_GPU_GetThermalSettings(hNvGPU[nToGet], 0, &thermal);
			if (ret == NVAPI_INCOMPATIBLE_STRUCT_VERSION)
			{
				InitThermal(thermal, NV_GPU_THERMAL_SETTINGS_VER_2);
				ret = NvAPI_GPU_GetThermalSettings(hNvGPU[nToGet], 0, &thermal);
			}
		}

		if (ret != NVAPI_OK)
		{
			nCurTemp[nToGet] = 0;
		}
		else
		{
			DWORD nTemp = 0;
			for (DWORD i = 0; i < thermal.count; ++i)
				nTemp += thermal.sensor[i].currentTemp;
			nTemp /= thermal.count;
			nCurTemp[nToGet] = nTemp;
			if (nCurTemp[nToGet] > nCurMaxTemp[nToGet]) nCurMaxTemp[nToGet] = nCurTemp[nToGet];
			nMaxTemp[nToGet] = thermal.sensor[0].defaultMaxTemp;
		}
	}
	static NvPhysicalGpuHandle hNvGPU[N_MAX_GPUS];
};

int Measure::iNvGPUInit = 0;
DWORD Measure::nCurTemp[N_MAX_GPUS];
DWORD Measure::nCurMaxTemp[N_MAX_GPUS];
DWORD Measure::nMaxTemp[N_MAX_GPUS];
DWORD Measure::nUsage[N_MAX_GPUS];
DWORD Measure::nMaxUsage[N_MAX_GPUS];
NvPhysicalGpuHandle Measure::hNvGPU[N_MAX_GPUS];
NvU32 Measure::nNvGPU;

#define UPDATE_GPUTEMP 0x01
#define UPDATE_GPUUSAGE 0x02

static DWORD WINAPI ThreadProc(LPVOID lpParameter)
{
	Measure* measure = (Measure*)lpParameter;

	while (1)
	{
		for (DWORD n = 0; n < measure->nNvGPU; n++)
		{
			if (bUpdate[n] & UPDATE_GPUTEMP)
			{
				if (!thread) return 0;
				measure->GetGPUTemperature(n);
				bUpdate[n] &= ~UPDATE_GPUTEMP;
			}
			if (bUpdate[n] & UPDATE_GPUUSAGE)
			{
				if (!thread) return 0;
				measure->GetGPUUsage(n);
				bUpdate[n] &= ~UPDATE_GPUUSAGE;
			}
		}		
		if (!thread) return 0;
		ResetEvent(hEvent);
		WaitForSingleObject(hEvent, INFINITE);
		if (!thread) return 0;
	}
}

PLUGIN_EXPORT void Initialize(void** data, void* rm)
{
	Measure* measure = new Measure;
	*data = measure;

	measure->nGPUToGet = RmReadInt(rm, L"Number", 0);

	if (RmReadInt(rm, L"NewThread", 0) > 0)
		measure->InitializeGPU(measure);
	else
		measure->InitializeGPU(NULL);

	measure->GetGPUTemperature(measure->nGPUToGet);

	BOOL showlog = RmReadInt(rm, L"Showlog", 0) > 0 ? true : false;

	if (!measure->advance)measure->advance = RmReadInt(rm, L"Advance", 0) > 0 ? true : false;

	if (showlog && measure->iNvGPUInit == 1)
	{
		measure->GetGPUName();
		if (measure->advance)RmLog(LOG_NOTICE, L"NvidiaGPU.dll: AdvanceMode");
	}
}

PLUGIN_EXPORT void Reload(void* data, void* rm, double* maxValue)
{
	Measure* measure = (Measure*)data;

	LPCWSTR value = RmReadString(rm, L"InfoType", L"Temperature");
	if (_wcsicmp(value, L"Temperature") == 0)
	{
		measure->type = GPU_CurTemp;
		*maxValue = measure->nMaxTemp[measure->nGPUToGet];
	}
	else if (_wcsicmp(value, L"MaxTemperature") == 0)
	{
		measure->type = GPU_CurMaxTemp;
		*maxValue = measure->nMaxTemp[measure->nGPUToGet];
	}
	else if (_wcsicmp(value, L"DefaultMaxTemp") == 0)
	{
		measure->type = GPU_MaxTemp;
	}
	else if (_wcsicmp(value, L"Usage") == 0)
	{
		measure->type = GPU_Usage;
	}
	else if (_wcsicmp(value, L"MaxUsage") == 0)
	{
		measure->type = GPU_MaxUsage;
	}
	else
	{
		measure->type = GPU_CurTemp;
		*maxValue = measure->nMaxTemp[measure->nGPUToGet];
		RmLog(LOG_ERROR, L"NvidiaGPU.dll: Invalid InfoType");
	}
}

PLUGIN_EXPORT double Update(void* data)
{
	Measure* measure = (Measure*)data;

	if (measure->type == GPU_CurTemp)
	{		
		if (thread)
		{
			bUpdate[measure->nGPUToGet] |= UPDATE_GPUTEMP;
			SetEvent(hEvent);
		}
		else measure->GetGPUTemperature(measure->nGPUToGet);
		return measure->nCurTemp[measure->nGPUToGet];
	}
	else if (measure->type == GPU_CurMaxTemp)
	{
		return measure->nCurMaxTemp[measure->nGPUToGet];
	}
	else if (measure->type == GPU_MaxTemp)
	{
		return measure->nMaxTemp[measure->nGPUToGet];
	}
	else if (measure->type == GPU_Usage)
	{
		if (thread)
		{
			bUpdate[measure->nGPUToGet] |= UPDATE_GPUUSAGE;
			SetEvent(hEvent);
		}
		else measure->GetGPUUsage(measure->nGPUToGet);
		return measure->nUsage[measure->nGPUToGet];
	}
	else if (measure->type == GPU_MaxUsage)
	{
		return measure->nMaxUsage[measure->nGPUToGet];
	}
	else return 0.0;
}

//PLUGIN_EXPORT LPCWSTR GetString(void* data)
//{
//	Measure* measure = (Measure*)data;
//	return measure->sReturn;
//}

//PLUGIN_EXPORT void ExecuteBang(void* data, LPCWSTR args)
//{
//	Measure* measure = (Measure*)data;
//}

PLUGIN_EXPORT void Finalize(void* data)
{
	Measure* measure = (Measure*)data;

	measure->iNvGPUInit -= 1;
	if (measure->iNvGPUInit < 1)measure->FinalizeGPU();

	delete measure;
}