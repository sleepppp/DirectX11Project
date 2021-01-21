#pragma once
class DepthStencilState
{
	Singleton(DepthStencilState)

	DepthStencilState();
public:
	enum class State : int
	{
		Default
	};
private:
	unordered_map<State, ID3D11DepthStencilState*> mStateList;
	typedef unordered_map<State, ID3D11DepthStencilState*>::iterator StateIter;

	State mLastState;
	ID3D11DepthStencilState* mLastStatePtr;
public:
	void AddState(State state, ID3D11DepthStencilState* statePtr);
	void Bind(State state);
};

