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
	bool GetKeyDown(int key);		//키가 눌렸다면
	bool GetKeyUp(int key);			//눌렀던 키에서 손을 땠다면
	bool GetKey(int key);			//키를 계속 누르고 있다면
	bool GetToggleKey(int key);		//토글

	D3DXVECTOR3 GetMousePosition()const { return mMousePosition; }
};

