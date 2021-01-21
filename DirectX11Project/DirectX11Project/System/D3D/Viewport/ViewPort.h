#pragma once
class ViewPort
{
	D3D11_VIEWPORT mViewport;
public:
	ViewPort();
	ViewPort(float left, float top, float width, float height, float min, float max);
	ViewPort(const D3D11_VIEWPORT& viewPort);

	void Resize(float width, float height);

	void Bind();
};

