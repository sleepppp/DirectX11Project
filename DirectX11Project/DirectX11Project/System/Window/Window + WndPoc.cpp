#include "stdafx.h"
#include "Window.h"
#include "System\Editor\EditorMain\EditorMain.h"

LRESULT Window::WndProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == WM_CLOSE || message == WM_DESTROY)
	{
		PostQuitMessage(0);

		return 0;
	}
	else if (message == WM_SIZE)
	{
		ImGui::Invalidate();
		float width = (float)LOWORD(lParam);
		float height = (float)HIWORD(lParam);

		if (D3D::GetInstance()->IsPerfect())
		{
			D3D::GetInstance()->OnResizeScreen(width, height);
		}

		ImGui::Validate();
	}
	else if (message == WM_MOUSEMOVE)
	{
		Input::GetInstance()->WndProc(handle, message, wParam, lParam);
	}

	if (ImGui::WndProc((UINT*)handle, message, wParam, lParam) == true)
		return true;

	return DefWindowProc(handle, message, wParam, lParam);
}
