#include "stdafx.h"
#include "Input.h"

/****************************************************
## Input ##
*****************************************************/
Input::Input()
{
	//ó���� ��� Ű�� �������� ���� ���·� �ʱ�ȭ
	mKeyCurrent.reset();
	mKeyPast.reset();

	mMousePosition.z = 0.f;
}

void Input::WndProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == WM_MOUSEMOVE)
	{
		mMousePosition.x = (float)LOWORD(lParam);
		mMousePosition.y = (float)HIWORD(wParam);
	}
}

/****************************************************
## Update ##
*****************************************************/
void Input::Update()
{
	//��� ����Ű�� ����Ű�� �������ش�
	mKeyPast = mKeyCurrent;
}


bool Input::GetKeyDown(int key)
{
	//�ش� Ű�� ���� ����
	if (GetAsyncKeyState(key) & 0x8000)
	{
		//�� Ű�� �������� �ʾҴٸ�
		if (!mKeyPast[key])
		{
			//����Ű�� ���� ���·� �ٲٰ� return true
			mKeyCurrent.set(key, true);
			return true;
		}
	}
	//�ش� Ű�� ������ ���� ����
	else mKeyCurrent.set(key, false);   //����Ű�� ������ ���� ���·� �ٲ�

	return false;
}

bool Input::GetKeyUp( int key)
{
	//�ش� Ű�� ���� ����					����Ű�� ���� ���·� �ٲ�
	if (GetAsyncKeyState(key) & 0x8000) mKeyCurrent.set(key, true);
	//�ش� Ű�� ������ ���� ����
	else
	{
		//�� Ű�� �����־��ٸ�
		if (mKeyPast[key])
		{
			//����Ű�� ������ ���� ���·� �ٲٰ� return true
			mKeyCurrent.set(key, false);
			return true;
		}
	}

	return false;
}

bool Input::GetKey(int key)
{
	if (GetAsyncKeyState(key) & 0x8000) return true;

	return false;
}

bool Input::GetToggleKey(int key)
{
	if (GetKeyState(key) & 0x0001) return true;

	return false;
}
