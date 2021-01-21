#pragma once
class Program
{
	friend class Window;
private:
	void Awake();
	void Start();

	void PreUpdate();
	void Update();
	void PostUpdate();

	void PreRender();
	void Render();
	void PostRender();

	void OnDestroy();
};

