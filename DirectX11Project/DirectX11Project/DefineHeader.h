#pragma once

#define SafeRelease(p){ if(p){ (p)->Release(); (p) = NULL; } }
#define SafeDelete(p){ if(p){ delete (p); (p) = NULL; } }
#define SafeDeleteArray(p){ if(p){ delete [] (p); (p) = NULL; } }

#define Synthesize(VarType, VarName, FuncName)	\
protected:	VarType VarName;	\
public:	inline VarType Get##FuncName(void) const { return VarName; }	\
public:	inline void Set##FuncName(VarType value) { VarName = value; }

#define Property(type, name)																	\
	public: __declspec(property(put = __typeToSet_##name, get = __typeToGet##name)) type name;	\
	void __typeToSet_##name(const type& value) { m_##name = value; }							\
	type __typeToGet##name() const { return m_##name; }											\
	private: type m##name;																		\

#define ReadonlyProperty(type, name)															\
	public: __declspec(property(get = __typeToGet_##name)) type name;							\
	type __typeToGet_##name() const { return m_##name; }										\
	private: type m##name;		

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

#define PathEditorData(path) (wstring(L"../EditorData/").append(path))
#define PathResources(path)(wstring(L"../Resources/").append(path))

