/* Copyright (C) 2017 Rainmeter Project Developers
 *
 * This Source Code Form is subject to the terms of the GNU General Public
 * License; either version 2 of the License, or (at your option) any later
 * version. If a copy of the GPL was not distributed with this file, You can
 * obtain one at <https://www.gnu.org/licenses/gpl-2.0.html>. */

#include <Windows.h>
#include "../API/RainmeterAPI.h"
#include <algorithm>
#include <vector>
#include <string>
#include "AIDA64Reader.h"

 // Overview: This example demonstrates a basic implementation of a parent/child
 // measure structure. In this particular example, we have a "parent" measure
 // which contains the values for the options "ValueA", "ValueB", and "ValueC".
 // The child measures are used to return a specific value from the parent.

 // Use case: You could, for example, have a "main" parent measure that queries
 // information from some data set. The child measures can then be used to return
 // specific information from the data queried by the parent measure.

 // Sample skin:
 /*
	 [Rainmeter]
	 Update=1000
	 DynamicWindowSize=1
	 BackgroundMode=2
	 SolidColor=255,255,255

	 [mParent]
	 Measure=Plugin
	 Plugin=ParentChild
	 ValueA=111
	 ValueB=222
	 ValueC=333
	 Type=A

	 [mChild1]
	 Measure=Plugin
	 Plugin=ParentChild
	 ParentName=mParent
	 Type=B

	 [mChild2]
	 Measure=Plugin
	 Plugin=ParentChild
	 ParentName=mParent
	 Type=C

	 [Text]
	 Meter=String
	 MeasureName=mParent
	 MeasureName2=mChild1
	 MeasureName3=mChild2
	 Text="mParent: %1#CRLF#mChild1: %2#CRLF#mChild2: %3"
 */

 //enum MeasureType
 //{
 //	MEASURE_A,
 //	MEASURE_B,
 //	MEASURE_C
 //};

//struct ChildMeasure;

class Measure
{
public:
	//string type = "";
	//bool is_updateMeasure = false;
	//static std::map<void*, UpdateMeasure*> g_updateMeasures;
	//static ofstream g_reportfile;
	//static std::mutex g_metex;
};

//std::map<void*, UpdateMeasure*> UpdateMeasure::g_updateMeasures;
//ofstream UpdateMeasure::g_reportfile;
//std::mutex UpdateMeasure::g_metex;


//static bool WCharToMByte(LPCWSTR lpcwszStr, string& str)
//{
//	int dwMinSize = 0;
//	LPSTR lpszStr = NULL;
//	dwMinSize = WideCharToMultiByte(CP_OEMCP, NULL, lpcwszStr, -1, NULL, 0, NULL, FALSE);
//	if (0 == dwMinSize)
//	{
//		return FALSE;
//	}
//	lpszStr = new char[dwMinSize];
//	WideCharToMultiByte(CP_OEMCP, NULL, lpcwszStr, -1, lpszStr, dwMinSize, NULL, FALSE);
//	str = lpszStr;
//	delete[] lpszStr;
//	return TRUE;
//}

PLUGIN_EXPORT void Initialize(void** data, void* rm)
{
	Measure* child = new Measure;
	*data = child;

	auto skin = RmGetSkin(rm);

	//if (UpdateMeasure::g_updateMeasures.find(skin) == UpdateMeasure::g_updateMeasures.end())
	//{
	//	UpdateMeasure::g_updateMeasures[skin] = child;
	//	child->is_updateMeasure = true;
	//}
}

PLUGIN_EXPORT void Reload(void* data, void* rm, double* maxValue)
{
	Measure* child = (Measure*)data;

	//WCharToMByte(RmReadString(rm, L"Type", L""), child->type);
	
	wstring path = RmReadString(rm, L"AIDA64Path", L"");
	if (!path.empty())
	{
		int needTerminate = RmReadInt(rm, L"NeedTerminate", 0);
		AIDA64Reader::getInstance()->runAIDA64(path, needTerminate);
	}
	//wstring report_path = RmReadString(rm, L"ReportPath", L"");
	/*if (!report_path.empty())
	{
		std::thread th1([report_path](){
			AIDA64Reader::getInstance()->reportFile(report_path);
			});
		th1.detach();
	}
	AIDA64Reader::getInstance()->waitForLoadMemory();*/
}

PLUGIN_EXPORT double Update(void* data)
{
	Measure* child = (Measure*)data;

	//if (child->type == "")
	//{
	//	return 0.0;
	//}

	//if (child->is_updateMeasure)
	//{
	//	AIDA64Reader::getInstance()->updateData();
	//}
	
	double ret = 0.0;
	//try
	//{
	//	ret = std::stod(AIDA64Reader::getInstance()->getValue(child->type));
	//}
	//catch (const std::exception&)
	//{
	//	wstring s = L"UpdateException\n";
	//	OutputDebugString(s.c_str());
	//	//RmLog(LOG_DEBUG);
	//}

	return ret;
}

PLUGIN_EXPORT void Finalize(void* data)
{
	Measure* child = (Measure*)data;

	//if (child->is_updateMeasure)
	//{
	//	for (auto it = UpdateMeasure::g_updateMeasures.begin(); it != UpdateMeasure::g_updateMeasures.end(); ++it)
	//	{
	//		if (it->second == child)
	//		{
	//			UpdateMeasure::g_updateMeasures.erase(it);
	//			break;
	//		}
	//	}
	//	//if (UpdateMeasure::g_updateMeasures.size() == 0)
	//	//{
	//	//	AIDA64Reader::getInstance()->release();
	//	//}
	//}
	delete child;
}
