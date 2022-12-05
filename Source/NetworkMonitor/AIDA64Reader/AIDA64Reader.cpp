#include "AIDA64Reader.h"
#include <sstream>
#include <tlhelp32.h>
//#include <iomanip>


AIDA64Reader::AIDA64Reader()
{
}

//bool AIDA64Reader::loadMemory()
//{
//	if (!mFileMappingHandle && !mShareMemory)
//	{
//		mFileMappingHandle = OpenFileMapping(FILE_MAP_READ, false, L"AIDA64_SensorValues");
//		if (mFileMappingHandle)
//		{
//			mShareMemory = MapViewOfFile(mFileMappingHandle, FILE_MAP_READ, 0, 0, 0);
//			if (!mShareMemory)
//			{
//				CloseHandle(mFileMappingHandle);
//				mFileMappingHandle = NULL;
//			}
//		}
//	}
//

//	return mShareMemory;
//}
//
//void AIDA64Reader::parseXMLData()
//{
//	TiXmlDocument mXMLParser;
//	memcpy(mDataString, mShareMemory, sizeof(mDataString));
//	mDataString[sizeof(mDataString) - 1] = '\0';
//	mXMLParser.Parse(mDataString);
//	string id = "";
//	string value = "";
//
//	TiXmlNode* rootElement = mXMLParser.FirstChildElement();
//	while (rootElement)
//	{
//		TiXmlNode* SubItem = rootElement->FirstChild();
//		while (SubItem)
//		{
//			TiXmlElement* ExtNameElement = SubItem->ToElement();
//			if (ExtNameElement)
//			{
//				if (ExtNameElement->ValueStr() == "id")
//					id = ExtNameElement->GetText();
//				if (ExtNameElement->ValueStr() == "value")
//					value = ExtNameElement->GetText();
//			}
//			SubItem = SubItem->NextSibling();
//		}
//		mMonitorData[id] = value;
//		rootElement = rootElement->NextSibling();
//	}
//}

AIDA64Reader::~AIDA64Reader()
{
	release();
}
//
//const string& AIDA64Reader::getValue(const string& key)
//{
//	shared_lock<shared_timed_mutex> lgd(mUpdateMutex);
//	return mMonitorData[key];
//}
//
//void AIDA64Reader::updateData()
//{
//	if (clock() - mTimestamp < 0.9f*CLOCKS_PER_SEC)
//	{
//		return;
//	}
//
//	unique_lock<shared_timed_mutex> lgd(mUpdateMutex);
//	if (clock() - mTimestamp >= 0.9f*CLOCKS_PER_SEC && loadMemory())
//	{
//		parseXMLData();
//		mTimestamp = clock();
//	}
//}

void AIDA64Reader::release()
{
	lock_guard<mutex> lgd(mMutex);
	//if (mShareMemory)
	//{
	//	UnmapViewOfFile(mShareMemory);
	//	mShareMemory = NULL;
	//}
	//if (mFileMappingHandle)
	//{
	//	CloseHandle(mFileMappingHandle);
	//	mFileMappingHandle = NULL;
	//}
	if (mAIDA64ProcessHandle)
	{
		if (mNeedTerminate)
		{
			TerminateProcess(mAIDA64ProcessHandle, -1);
		}
		CloseHandle(mAIDA64ProcessHandle);
		mAIDA64ProcessHandle = NULL;
	}
}

bool AIDA64Reader::runAIDA64(const wstring& file_path, bool need_terminate)
{
	lock_guard<mutex> lgd(mMutex);
	if (mAIDA64ProcessHandle || findAIDA64())
	{
		return true;
	}
	else
	{
		if (file_path.find(L"aida64.exe") != file_path.npos)
		{
			SHELLEXECUTEINFO ShExecInfo;

			ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
			ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
			ShExecInfo.hwnd = NULL;
			ShExecInfo.lpVerb = NULL;
			ShExecInfo.lpFile = file_path.c_str();
			ShExecInfo.lpParameters = NULL;
			ShExecInfo.lpDirectory = NULL;
			ShExecInfo.nShow = SW_SHOWNOACTIVATE;
			ShExecInfo.hInstApp = NULL;

			if (ShellExecuteEx(&ShExecInfo))
			{
				mAIDA64ProcessHandle = ShExecInfo.hProcess;
				mNeedTerminate = need_terminate;
				return true;
			}
		}
	}
	return false;
}

bool AIDA64Reader::findAIDA64()
{
	//DWORD id = 0;
	if (mAIDA64ProcessHandle)
	{
		return true;
	}
	HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32 pInfo; 
	pInfo.dwSize = sizeof(pInfo);
	Process32First(hSnapShot, &pInfo);
	do
	{
		wstring s = pInfo.szExeFile;
		if (s == L"aida64.exe")
		{
			//mAIDA64ProcessHandle = OpenProcess(PROCESS_ALL_ACCESS, true, pInfo.th32ProcessID);
			return true;
		}
	} while (Process32Next(hSnapShot, &pInfo) != FALSE);
	return false;
}

//void AIDA64Reader::reportFile(const wstring& file_path)
//{
//	wstring s = L"reportFileStart\n";
//	OutputDebugString(s.c_str());
//	waitForLoadMemory();
//
//	wofstream file(file_path, ios::out);
//	if (file.is_open())
//	{
//		TiXmlDocument mXMLParser;
//
//		mUpdateMutex.lock_shared();
//		mXMLParser.Parse(mDataString);
//		mUpdateMutex.unlock_shared();
//		try
//		{
//			file << "id,name,value" << endl;
//			TiXmlNode* rootElement = mXMLParser.FirstChildElement();
//			while (rootElement)
//			{
//				TiXmlNode* SubItem = rootElement->FirstChild();
//				while (SubItem)
//				{
//					TiXmlElement* ExtNameElement = SubItem->ToElement();
//					if (ExtNameElement)
//					{
//						file << ExtNameElement->GetText() << ",";
//					}
//					SubItem = SubItem->NextSibling();
//				}
//				file << endl;
//				rootElement = rootElement->NextSibling();
//			}
//			file << setw(10) << " ";
//		}
//		catch (const std::exception&)
//		{
//		}
//	}
//	s = L"reportFileEnd\n";
//	OutputDebugString(s.c_str());
//}
//
//bool AIDA64Reader::exists() const
//{
//	return mShareMemory;
//}
//
//void AIDA64Reader::waitForLoadMemory()
//{
//	while (!mShareMemory)
//	{
//		wstring s = L"waitForShareMemory\n";
//		OutputDebugString(s.c_str());
//		mUpdateMutex.lock();
//		loadMemory();
//		mUpdateMutex.unlock();
//		Sleep(100);
//	}
//	while (strlen(mDataString)==0)
//	{
//		wstring s = L"waitForReadString\n";
//		OutputDebugString(s.c_str());
//		mUpdateMutex.lock();
//		memcpy(mDataString, mShareMemory, sizeof(mDataString));
//		mDataString[sizeof(mDataString) - 1] = '\0';
//		mUpdateMutex.unlock();
//		Sleep(100);
//	}
//	Sleep(2000);
//}
