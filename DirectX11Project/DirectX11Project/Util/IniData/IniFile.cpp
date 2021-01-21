#include "stdafx.h"
#include "IniFile.h"

#include <fstream>

IniFile::IniFile(const wstring & fileName)
	:mFileName(fileName) 
{
	//파일 없다면 생성
	HANDLE handle = CreateFile(fileName.c_str(), GENERIC_READ, 0, NULL, OPEN_ALWAYS,
		FILE_ATTRIBUTE_NORMAL, NULL);

	if (handle == NULL)
	{
		handle = CreateFile(fileName.c_str(), GENERIC_READ, 0, NULL, CREATE_NEW,
			FILE_ATTRIBUTE_NORMAL, NULL);
	}

	if(handle)
		CloseHandle(handle);
}

void IniFile::AddData(const wstring & subject, const wstring & title, const wstring & body)
{
	IniData newData(subject, title, body);

	IniIter iter = mDataList.find(subject);
	if (iter == mDataList.end())
	{
		mDataList.insert(make_pair(subject, vector<IniData>()));
		iter = mDataList.find(subject);
	}
	
	//if (IteratorHelper::IsContain<IniData>(iter->second, newData))return;

	for (int i = 0; i < iter->second.size(); ++i)
	{
		if (iter->second[i].Title == title)
		{
			iter->second[i] = newData;
			return;
		}
	}

	iter->second.emplace_back(newData);
}

void IniFile::RemoveData(const wstring & subject, const wstring & title)
{
	IniIter iter = mDataList.find(subject);

	if (iter == mDataList.end())
		return;

	for (int i = 0; i < iter->second.size(); ++i)
	{
		if (iter->second[i].Title == title)
		{
			iter->second.erase(iter->second.begin() + i);
			return;
		}
	}
}

wstring IniFile::GetData(const wstring & subject, const wstring & title)
{
	WCHAR str[65];

	IniIter iter = mDataList.find(subject);
	if (iter != mDataList.end())
	{
		for (int i = 0; i < iter->second.size(); ++i)
		{
			if (iter->second[i].Title == title)
			{
				return iter->second[i].Body;
			}
		}

		//여기까지 왔다는 것은 없는 처음 로드하는 데이터라는 뜻
		GetPrivateProfileString(subject.c_str(), title.c_str(), L"", str, 64, mFileName.c_str());

		iter->second.emplace_back(IniData(subject,title,str));
	}
	else
	{
		//여기까지 왔다는 것은 없는 처음 로드하는 데이터라는 뜻
		GetPrivateProfileString(subject.c_str(), title.c_str(), L"", str, 64, mFileName.c_str());

		vector<IniData> list;
		list.emplace_back(IniData(subject, title, str));
		mDataList.insert(make_pair(subject, list));
	}

	return str;
}

void IniFile::Save()
{
	IniIter iter = mDataList.begin();
	for (; iter != mDataList.end(); ++iter)
	{
		for (int i = 0; i < iter->second.size(); ++i)
		{
			WritePrivateProfileString(iter->second[i].Subject.c_str(),
				iter->second[i].Title.c_str(), iter->second[i].Body.c_str(), mFileName.c_str());
		}
	}
}

void IniFile::Clear()
{
	mDataList.clear();
}
