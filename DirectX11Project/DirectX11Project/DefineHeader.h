#pragma once

#define SafeRelease(p){ if(p){ (p)->Release(); (p) = NULL; } }
#define SafeDelete(p){ if(p){ delete (p); (p) = NULL; } }
#define SafeDeleteArray(p){ if(p){ delete [] (p); (p) = NULL; } }

#define Synthesize(VarType, VarName, FuncName)	\
protected:	VarType VarName;	\
public:	inline VarType Get##FuncName(void) const { return VarName; }	\
public:	inline void Set##FuncName(VarType value) { VarName = value; }

#define Singleton(ClassName)\
public:\
static ClassName* GetInstance()\
{\
	static ClassName instance;\
	return &instance;\
}

#define GetName(t) (#t)

#define FEPSLON		0.00001f
#define FLOATQUAL(a, b)     (abs((a)- (b)) <FEPSLON)
#define FLOATZERO(a)		(abs(a) < FEPSLON)
#define VECTORZERO(v)		(FLOATZERO((v).x) && FLOATZERO((v).y)&& FLOATZERO((v).z))

inline void Assert(HRESULT hr) { assert(SUCCEEDED(hr)); }
inline void ErrorMessage(HWND hwnd ,const wstring& str)
{
	MessageBox(hwnd, str.c_str(), str.c_str(), MB_ICONERROR);
}