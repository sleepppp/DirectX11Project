#include "stdafx.h"
#include "ViewPort.h"

ViewPort::ViewPort()
{
	ZeroMemory(&mViewport, sizeof(D3D11_VIEWPORT));
}

ViewPort::ViewPort(float left, float top, float width, float height, float min, float max)
{
	mViewport.TopLeftX = left;
	mViewport.TopLeftY = top;
	mViewport.Width = width;
	mViewport.Height = height;
	mViewport.MinDepth = min;
	mViewport.MaxDepth = max;
}

ViewPort::ViewPort(const D3D11_VIEWPORT & viewPort)
{
	mViewport = viewPort;
}

void ViewPort::Resize(float width, float height)
{
	mViewport.Width = width;
	mViewport.Height = height;
}

void ViewPort::Bind()
{
	D3DDeviceContext->RSSetViewports(1, &mViewport);
}
