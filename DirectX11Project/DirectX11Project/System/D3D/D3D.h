#pragma once

class D3D final
{
	Singleton(D3D)
private:
	ID3D11Device* mDevice;
	ID3D11DeviceContext* mDeviceContext;
	IDXGISwapChain* mSwapChain;

	class RenderTarget* mRenderTarget;
	class ViewPort* mViewPort;

	UINT mGpuMemorySize;
	wstring mGPUDescription;
	UINT mNumerator;
	UINT mDenominator;
private:
	D3D();
	~D3D();

	void SetGPUInfo();
	void CreateSwapChainAndDevice();
	void CreateBackBuffer();
public:
	ID3D11Device* GetDevice()const { return mDevice; }
	ID3D11DeviceContext* GetDeviceContext()const { return mDeviceContext; }
	class RenderTarget* GetRenderTarget()const { return mRenderTarget; }

	void Init();
	void BeginRender();
	void EndRender();
	void OnResizeScreen(float width, float height);
	bool IsPerfect()const;
};

#define D3DDevice D3D::GetInstance()->GetDevice()
#define D3DDeviceContext D3D::GetInstance()->GetDeviceContext()