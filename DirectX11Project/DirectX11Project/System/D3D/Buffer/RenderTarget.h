#pragma once
class RenderTarget final
{
private:
	ID3D11DepthStencilView* mDepthStencilView;
	ID3D11RenderTargetView* mRenderTargetView;

	ID3D11Texture2D* mDepthTexture2D;
public:
	RenderTarget(float width, float height, IDXGISwapChain* swapChain);
	~RenderTarget();

	ID3D11DepthStencilView* GetDepthStencilView()const { return mDepthStencilView; }
	ID3D11RenderTargetView* GetRenderTargetView()const { return mRenderTargetView; }

	void Bind();
	void Clear(D3DXCOLOR color);
};

