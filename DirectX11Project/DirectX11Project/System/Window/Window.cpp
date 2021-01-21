#include "stdafx.h"
#include "Window.h"
#include "./System/Program/Program.h"
#include "./System/D3D/D3D.h"
#include "./Util/IniData/IniFile.h"
#include "./Util/StringHelper/StringHelper.h"

Window::Window()
{
	WindowDesc::CreateWindowDesc();
	WindowDesc::LoadWindowDesc(); 

	MakeWindow();
}

Window::~Window()
{
	if (WindowDesc::GetIsFullScreen() == true)
		ChangeDisplaySettings(NULL, 0);

	DestroyWindow(WindowDesc::GetHwnd());

	UnregisterClass(WindowDesc::GetWindowName().c_str(), WindowDesc::GetInstance());

	WindowDesc::SaveWindowDesc();
	WindowDesc::ReleaseWindowDesc();
}

void Window::MakeWindow()
{
	WindowDesc desc;
	WindowDesc::GetDesc(&desc);

	WNDCLASSEX wndClass;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wndClass.hIconSm = wndClass.hIcon;
	wndClass.hInstance = desc.Instance;
	wndClass.lpfnWndProc = (WNDPROC)WndProc;
	wndClass.lpszClassName = desc.AppName.c_str();
	wndClass.lpszMenuName = NULL;
	wndClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wndClass.cbSize = sizeof(WNDCLASSEX);

	WORD wHr = RegisterClassEx(&wndClass);
	assert(wHr != 0);

	if (WindowDesc::GetIsFullScreen() == true)
	{
		DEVMODE devMode = { 0 };
		devMode.dmSize = sizeof(DEVMODE);
		devMode.dmPelsWidth = (DWORD)desc.Width;
		devMode.dmPelsHeight = (DWORD)desc.Height;
		devMode.dmBitsPerPel = 32;
		devMode.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		ChangeDisplaySettings(&devMode, CDS_FULLSCREEN);
	}

	desc.Handle = CreateWindowEx
	(
		WS_EX_APPWINDOW
		, desc.AppName.c_str()
		, desc.AppName.c_str()
		, WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_OVERLAPPEDWINDOW
		, CW_USEDEFAULT
		, CW_USEDEFAULT
		, CW_USEDEFAULT
		, CW_USEDEFAULT
		, NULL
		, (HMENU)NULL
		, desc.Instance
		, NULL
	);
	assert(desc.Handle != NULL);

	RECT rect = { 0, 0, (LONG)desc.Width, (LONG)desc.Height };

	UINT centerX = (GetSystemMetrics(SM_CXSCREEN) - (UINT)desc.Width) / 2;
	UINT centerY = (GetSystemMetrics(SM_CYSCREEN) - (UINT)desc.Height) / 2;

	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);
	MoveWindow
	(
		desc.Handle
		, centerX, centerY
		, rect.right - rect.left, rect.bottom - rect.top
		, TRUE
	);
	ShowWindow(desc.Handle, SW_SHOWNORMAL);
	SetForegroundWindow(desc.Handle);
	SetFocus(desc.Handle);

	ShowCursor(true);

	WindowDesc::SetDesc(&desc);
}


WPARAM Window::Run()
{
	MSG msg = { 0 };

	mProgram = new Program();
	mProgram->Awake();
	mProgram->Start();

	while (true)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				break;

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			mProgram->PreUpdate();
			mProgram->Update();
			mProgram->PostUpdate();

			mProgram->PreRender();
			mProgram->Render();
			mProgram->PostRender();
		}
	}

	mProgram->OnDestroy();
	SafeDelete(mProgram);

	return msg.wParam;
}


