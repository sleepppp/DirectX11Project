#include "stdafx.h"
#include "D3D.h"

#include "Buffer\RenderTarget.h"
#include "Viewport\ViewPort.h"
#include "State\DepthStencilState.h"
#include "System\Editor\EditorMain\EditorMain.h"
D3D::D3D()
	:mNumerator(0), mDenominator(1), mRenderTarget(nullptr), mViewPort(nullptr),
	mDevice(nullptr), mDeviceContext(nullptr), mSwapChain(nullptr) {}


D3D::~D3D()
{
	SafeDelete(mViewPort);
	SafeDelete(mRenderTarget);

	if (mSwapChain)
		mSwapChain->SetFullscreenState(false, NULL);

	SafeRelease(mDeviceContext);
	SafeRelease(mDevice);
	SafeRelease(mSwapChain);
}

void D3D::SetGPUInfo()
{
	IDXGIFactory* factory;
	Assert(CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory));

	IDXGIAdapter* adapter;
	Assert(factory->EnumAdapters(0, &adapter));

	IDXGIOutput* adapterOutput;
	Assert(adapter->EnumOutputs(0, &adapterOutput));

	UINT modeCount;
	Assert(adapterOutput->GetDisplayModeList
	(
		DXGI_FORMAT_R8G8B8A8_UNORM,
		DXGI_ENUM_MODES_INTERLACED,
		&modeCount,
		NULL
	));

	DXGI_MODE_DESC* displayModeList = new DXGI_MODE_DESC[modeCount];
	Assert(adapterOutput->GetDisplayModeList
	(
		DXGI_FORMAT_R8G8B8A8_UNORM
		, DXGI_ENUM_MODES_INTERLACED
		, &modeCount
		, displayModeList
	));

	for (UINT i = 0; i < modeCount; ++i)
	{
		if (displayModeList[i].Width == (UINT)WinSizeX &&
			displayModeList[i].Height == (UINT)WinSizeY)
		{
			mNumerator = displayModeList[i].RefreshRate.Numerator;
			mDenominator = displayModeList[i].RefreshRate.Denominator;
		}
	}

	DXGI_ADAPTER_DESC adapterDesc;
	Assert(adapter->GetDesc(&adapterDesc));

	mGpuMemorySize = adapterDesc.DedicatedVideoMemory / 1024 / 1024;
	mGPUDescription = adapterDesc.Description;

	SafeDeleteArray(displayModeList);

	SafeRelease(adapterOutput);
	SafeRelease(adapter);
	SafeRelease(factory);
}

void D3D::CreateSwapChainAndDevice()
{
	WindowDesc windowDesc;
	WindowDesc::GetDesc(&windowDesc);

	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));

	swapChainDesc.BufferCount = 1;
	swapChainDesc.BufferDesc.Width = 0;
	swapChainDesc.BufferDesc.Height = 0;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	if (windowDesc.bVsync == true)
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = mNumerator;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = mDenominator;
	}
	else
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	}

	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow = windowDesc.Handle;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.Windowed = !windowDesc.bFullScreen;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapChainDesc.Flags = 0;

	UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#ifdef _DEBUG
	//creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_FEATURE_LEVEL featureLevels[] =
	{
		//D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL_9_2,
		D3D_FEATURE_LEVEL_9_1
	};

	Assert(D3D11CreateDeviceAndSwapChain
	(
		NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		creationFlags,
		featureLevels,
		1,
		D3D11_SDK_VERSION,
		&swapChainDesc,
		&mSwapChain,
		&mDevice,
		NULL,
		&mDeviceContext
	));

}

void D3D::CreateBackBuffer()
{
	mRenderTarget = new RenderTarget(WinSizeX, WinSizeY,mSwapChain);
	mViewPort = new ViewPort(0, 0, WinSizeX, WinSizeY, 0.f, 1.f);

	DepthStencilState::GetInstance()->Bind(DepthStencilState::State::Default);
}

void D3D::Init()
{
	SetGPUInfo();
	CreateSwapChainAndDevice();
	CreateBackBuffer();
}

void D3D::BeginRender()
{
	const float clearColor[] = { 0.3f,0.3f,0.3f,1.f };
	//static const D3DXCOLOR clearColor = D3DXCOLOR(0.3f, 0.3f, 0.3f, 1.f);
	mRenderTarget->Bind();
	mRenderTarget->Clear(clearColor);
	mViewPort->Bind();
}

void D3D::EndRender()
{
#ifdef _DEBUG
	Assert(mSwapChain->Present(0, 0));
#else
	mSwapChain->Present();
#endif
}

void D3D::OnResizeScreen(float width, float height)
{
	if (D3D::GetInstance()->IsPerfect() == false)return;

	WindowDesc desc;
	WindowDesc::GetDesc(&desc);
	{
		desc.Width = width;
		desc.Height = height;
	}
	WindowDesc::SetDesc(&desc);

	SafeDelete(mRenderTarget);

	Assert(mSwapChain->ResizeBuffers(0, (UINT)width, (UINT)height, DXGI_FORMAT_UNKNOWN, 0));

	mRenderTarget = new RenderTarget(width, height, mSwapChain);
	mViewPort->Resize(width, height);
}

bool D3D::IsPerfect() const
{
	if (mDevice == nullptr)return false;
	if (mDeviceContext == nullptr)return false;
	if (mSwapChain == nullptr)return false;
	
	return true;
}
