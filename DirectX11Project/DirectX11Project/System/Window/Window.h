#pragma once
#include "WindowDesc\WindowDesc.h"

class Window
{
private:
	class Program* mProgram;
public:
	Window();
	~Window();

	void MakeWindow();

	WPARAM Run();
private:
	static LRESULT CALLBACK WndProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam);
};

