#include "stdafx.h"
#include "DepthStencilState.h"

DepthStencilState::DepthStencilState()
{
	D3D11_DEPTH_STENCIL_DESC desc;
	desc.DepthEnable = true;
	desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	desc.DepthFunc = D3D11_COMPARISON_LESS;

	desc.StencilEnable = true;
	desc.StencilReadMask = 0xFF;
	desc.StencilWriteMask = 0xFF;

	desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	desc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	desc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	desc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	ID3D11DepthStencilState* state;
	Assert(D3DDevice->CreateDepthStencilState(&desc, &state));

	mLastState = State::Default;
	mLastStatePtr = state;

	AddState(mLastState, mLastStatePtr);
	D3DDeviceContext->OMSetDepthStencilState(mLastStatePtr, 0);
}

void DepthStencilState::AddState(State state, ID3D11DepthStencilState * statePtr)
{
	mStateList.insert(make_pair(state, statePtr));
}

void DepthStencilState::Bind(State state)
{
	if (mLastState != state)
	{
		StateIter iter = mStateList.find(state);
		if (iter != mStateList.end())
		{
			mLastState = state;
			mLastStatePtr = iter->second;

			D3DDeviceContext->OMSetDepthStencilState(mLastStatePtr, 0);
		}
	}
}
