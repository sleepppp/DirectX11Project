#pragma once
class WindowDesc
{
	static WindowDesc _windowDesc;
	static wstring _windowDescIniPath;
	static class IniFile* _WindowDescIniFile;
public:
	static void CreateWindowDesc();
	static void LoadWindowDesc();
	static void SaveWindowDesc();
	static void ReleaseWindowDesc();

	static void GetDesc(WindowDesc* const pDesc);
	static void SetDesc(const WindowDesc*const pDesc);

	static float GetWindowSizeX() { return _windowDesc.Width; }
	static float GetWindowSizeY() { return _windowDesc.Height; }
	static bool GetIsFullScreen() { return _windowDesc.bFullScreen; }
	static bool GetIsVSync() { return _windowDesc.bVsync; }
	static HWND GetHwnd() { return _windowDesc.Handle; }
	static wstring GetWindowDescFilePath() { return _windowDescIniPath; }
	static wstring GetWindowName() { return _windowDesc.AppName; }
	static HINSTANCE GetInstance() { return _windowDesc.Instance; }
public:
	wstring AppName;
	HINSTANCE Instance;
	HWND Handle;
	float Width;
	float Height;
	bool bVsync;
	bool bFullScreen;
};

#define WinSizeX WindowDesc::GetWindowSizeX()
#define WinSizeY WindowDesc::GetWindowSizeY()