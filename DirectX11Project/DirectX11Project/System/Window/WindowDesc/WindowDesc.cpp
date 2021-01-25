#include "stdafx.h"
#include "WindowDesc.h"
#include "Util\IniData\IniFile.h"
#include "Util\StringHelper\StringHelper.h"

WindowDesc WindowDesc::_windowDesc;
wstring WindowDesc::_windowDescIniPath = L"../EditorData/WindowDesc.ini";
IniFile* WindowDesc::_WindowDescIniFile = nullptr;

void WindowDesc::CreateWindowDesc()
{
	_WindowDescIniFile = new IniFile(_windowDescIniPath);
}

void WindowDesc::LoadWindowDesc()
{
	wstring isFirstCompile = _WindowDescIniFile->GetData(L"Main", L"IsFirst");

	//처음 에디터를 컴파일했다는 뜻
	if (isFirstCompile != IniFile::False)
	{
		_windowDesc.AppName = L"DirectX11Project";
		_windowDesc.bFullScreen = false;
		_windowDesc.bVsync = false;
		_windowDesc.Width = 1280;
		_windowDesc.Height = 720;

		SaveWindowDesc();
	}
	else
	{
		wstring windowDescName = GetWName(WindowDesc);
		_windowDesc.AppName = _WindowDescIniFile->GetData(windowDescName.c_str(), L"AppName");
		_windowDesc.bFullScreen = (bool)stoi(_WindowDescIniFile->GetData(windowDescName.c_str(), L"bFullScreen"));
		_windowDesc.bVsync = (bool)stoi(_WindowDescIniFile->GetData(windowDescName.c_str(), L"bVsync"));
		_windowDesc.Width = (float)stoi(_WindowDescIniFile->GetData(windowDescName.c_str(), L"Width"));
		_windowDesc.Height = (float)stoi(_WindowDescIniFile->GetData(windowDescName.c_str(), L"Height"));
	}
}

void WindowDesc::SaveWindowDesc()
{
	_WindowDescIniFile->AddData(L"Main", L"IsFirst", IniFile::False);
	_WindowDescIniFile->AddData(L"WindowDesc", L"AppName", _windowDesc.AppName);
	_WindowDescIniFile->AddData(L"WindowDesc", L"bFullScreen", to_wstring((int)_windowDesc.bFullScreen));
	_WindowDescIniFile->AddData(L"WindowDesc", L"bVsync", to_wstring((int)_windowDesc.bVsync));
	_WindowDescIniFile->AddData(L"WindowDesc", L"Width", to_wstring(_windowDesc.Width));
	_WindowDescIniFile->AddData(L"WindowDesc", L"Height", to_wstring(_windowDesc.Height));

	_WindowDescIniFile->Save();
}

void WindowDesc::ReleaseWindowDesc()
{
	SafeDelete(_WindowDescIniFile);
}

void WindowDesc::GetDesc(WindowDesc* const pDesc)
{
	*pDesc = _windowDesc;
}

void WindowDesc::SetDesc(const WindowDesc*const pDesc)
{
	_windowDesc = *pDesc;
}
