#pragma once
template<class T>
class TSingleton
{
	static T* _instance;
public:
	static T* GetInstance()
	{
		if (_instance == nullptr)
		{
			_instance = new T();
		}
		return _instance;
	}

	static void ReleaseInstance()
	{
		SafeDelete(_instance);
	}
};

template<typename T>
T* TSingleton<T>::_instance = nullptr;

