// DirectX11Project.cpp : 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include "DirectX11Project.h"
#include "./System/Window/Window.h"
#include "./Util/IniData/IniFile.h"

int WINAPI WinMain(HINSTANCE instance, HINSTANCE prevInstance, LPSTR param, int command)
{
	_wsetlocale(LC_ALL, L"korean");
	srand((UINT)time(NULL));

	WindowDesc desc;
	desc.Instance = instance;
	desc.Handle = nullptr;
	WindowDesc::SetDesc(&desc);

	Window* window = new Window();
	WPARAM wParam = window->Run();
	SafeDelete(window);

	return wParam;
}
