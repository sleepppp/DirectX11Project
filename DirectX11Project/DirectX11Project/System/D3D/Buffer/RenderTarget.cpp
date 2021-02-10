#include "stdafx.h"
#include "RenderTarget.h"

/*******************************************************************************
## RenderTarget ##
@@ width : 가로길이
@@ height : 세로길이
@@ IDXGISwapChain : 스왑체인

스왑체인 전용 렌더타겟 생성
********************************************************************************/
RenderTarget::RenderTarget(float width, float height, IDXGISwapChain * swapChain)
{
#ifdef _DEBUG
	if (swapChain == nullptr)
	{
		ErrorMessage(WindowDesc::GetHwnd(),L"No Swapchain");
		return;
	}
#endif
	swapChain->ResizeBuffers(1, (UINT)WinSizeX, (UINT)WinSizeY, 
		DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, 0);

	ID3D11Texture2D* backBufferPointer;
	Assert(swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBufferPointer));
	Assert(D3DDevice->CreateRenderTargetView(backBufferPointer, NULL, &mRenderTargetView));
	SafeRelease(backBufferPointer);

	D3D11_TEXTURE2D_DESC descDepth;
	ZeroMemory(&descDepth, sizeof(descDepth));
	descDepth.Width = width;
	descDepth.Height = height;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;
	Assert(D3DDevice->CreateTexture2D(&descDepth, nullptr, &mDepthTexture2D));

	D3D11_DEPTH_STENCIL_VIEW_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
	desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	desc.Texture2D.MipSlice = 0;

	Assert(D3DDevice->CreateDepthStencilView(mDepthTexture2D, &desc, &mDepthStencilView));
}

/*******************************************************************************
## ~RenderTarget ##
********************************************************************************/
RenderTarget::~RenderTarget()
{
	SafeRelease(mDepthStencilView);
	SafeRelease(mRenderTargetView);
	SafeRelease(mDepthTexture2D);
}

/*******************************************************************************
## BindRenderTarget ##
********************************************************************************/
void RenderTarget::Bind()
{
#ifdef _DEBUG
	//if (mRenderTargetView == nullptr)
	//{
	//	ErrorMessage(Window::GetHwnd(),L"No RenderTargetView");
	//	return;
	//}
	//
	//if (mDepthStencilView == nullptr)
	//{
	//	ErrorMessage(Window::GetHwnd(),L"No DepthStencilView");
	//	return;
	//}
#endif

	D3DDeviceContext->OMSetRenderTargets(1, &mRenderTargetView, mDepthStencilView);
}

/*******************************************************************************
## clear ##
********************************************************************************/
void RenderTarget::Clear(const float color[])
{
#ifdef _DEBUG
	//if (mRenderTargetView == nullptr)
	//{
	//	ErrorMessage(Window::GetHwnd(),L"No RenderTargetView");
	//	return;
	//}
	//
	//if (mDepthStencilView == nullptr)
	//{
	//	ErrorMessage(Window::GetHwnd(),L"No DepthStencilView");
	//	return;
	//}
#endif
	
	D3DDeviceContext->ClearRenderTargetView(mRenderTargetView, color);
	D3DDeviceContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH | 
		D3D11_CLEAR_STENCIL, 1, 0);
}
