#pragma once
//#ifndef TIXML_USE_STL
//#define TIXML_USE_STL
//#endif 

#include <Windows.h>
#include <iostream>
//#include <fstream>
//#include <ctime>
//#include <map>
#include <mutex>
//#include "TinyXML\tinyxml.h"

using namespace std;

class AIDA64Reader
{
public:
	~AIDA64Reader();
	//const string& getValue(const string& key) ;
	//void updateData();
	bool runAIDA64(const wstring& file_path,bool need_terminate = false);
	bool findAIDA64();
	//void reportFile(const wstring&  file_path);
	//bool exists() const;
	void release();
	//void waitForLoadMemory();
	static AIDA64Reader* getInstance() 
	{
		static AIDA64Reader instance;
		return &instance;
	}

private:
	AIDA64Reader();
	//bool loadMemory();
	//void parseXMLData();

	//system resource
	//LPVOID mShareMemory = NULL;
	HANDLE mAIDA64ProcessHandle = NULL;
	//HANDLE mFileMappingHandle = NULL;
	bool mNeedTerminate = false;

	//multithread
	//clock_t mTimestamp = 0;
	mutex mMutex;

	//data
	//map<string, string> mMonitorData;
	//char mDataString[8192] = "";
};

