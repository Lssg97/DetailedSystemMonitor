#include <Windows.h>
#include "../API/RainmeterAPI.h"
#include "NetworkReader.h"

// Overview: This is a blank canvas on which to build your plugin.

// Note: GetString, ExecuteBang and an unnamed function for use as a section variable
// have been commented out. Uncomment any functions as needed.
// For more information, see the SDK docs: https://docs.rainmeter.net/developers/plugin/cpp/

class Measure
{
public:
	Measure() {}
	int index = 0;
	std::wstring process = L"";
	ULONG64 bytes = 0;
};

PLUGIN_EXPORT void Initialize(void** data, void* rm)
{
	Measure* measure = new Measure;
	*data = measure;
}

PLUGIN_EXPORT void Reload(void* data, void* rm, double* maxValue)
{
	Measure* measure = (Measure*)data;
	measure->index = RmReadInt(rm, L"Index", 0);
}

PLUGIN_EXPORT double Update(void* data)
{
	Measure* measure = (Measure*)data;
	std::pair<std::wstring, ULONG64> _pair = NetworkReader::getInstance()->getPerformance(measure->index - 1);
	measure->process = _pair.first;
	measure->bytes = _pair.second;
	return measure->bytes;
}

PLUGIN_EXPORT LPCWSTR GetString(void* data)
{
	Measure* measure = (Measure*)data;
	//LPCWSTR tmp = NetworkReader::getInstance()->getPerformance(measure->index - 1).first.c_str();
	return measure->process.c_str();
}

//PLUGIN_EXPORT void ExecuteBang(void* data, LPCWSTR args)
//{
//	Measure* measure = (Measure*)data;
//}

//PLUGIN_EXPORT LPCWSTR (void* data, const int argc, const WCHAR* argv[])
//{
//	Measure* measure = (Measure*)data;
//	return nullptr;
//}

PLUGIN_EXPORT void Finalize(void* data)
{
	Measure* measure = (Measure*)data;
	delete measure;
}
