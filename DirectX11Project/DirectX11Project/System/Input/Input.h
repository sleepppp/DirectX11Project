#pragma once
#include <bitset>

#define KEYMAX 256

class Input 
{
	Singleton(Input)
	friend class Window;
private:
	std::bitset<KEYMAX> mKeyPast;
	std::bitset<KEYMAX> mKeyCurrent;

	D3DXVECTOR3 mMousePosition;
private:
	Input();
	~Input() {}
	void WndProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam);
public:
	void Update();
	bool GetKeyDown(int key);		//Ű�� ���ȴٸ�
	bool GetKeyUp(int key);			//������ Ű���� ���� ���ٸ�
	bool GetKey(int key);			//Ű�� ��� ������ �ִٸ�
	bool GetToggleKey(int key);		//���

	D3DXVECTOR3 GetMousePosition()const { return mMousePosition; }
};

