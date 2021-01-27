#pragma once
#include <limits>
#include <Windows.h>
#include <string>
#include <functional>
#include <sstream>
#include <utility>

namespace Reflection
{
	class ClassUnknown;

	//**************************************************************************
	class Value
	{
	protected:
		virtual void* ValueDataReference() { return nullptr; }
	public:
		Value() = default;
		virtual ~Value() = default;

		virtual ClassUnknown& GetClass()const;

		template<typename T> operator T&();
		template<typename T> operator T*();

		template<typename R, typename...P> std::function<R(P...)>& FunctionObject();

		virtual bool IsVoid()const { return true; }
		virtual bool IsNumber()const { return false; }
		virtual bool IsBool()const { return false; }
		virtual bool IsString()const { return false; }
		virtual bool IsFunction()const { return false; }

		virtual int GetIntValue()const { return std::numeric_limits<int>::quiet_NaN(); }
		virtual void Setvalue(int value) {}

		virtual INT64 GetInt64Value()const { return std::numeric_limits<INT64>::quiet_NaN(); }
		virtual void SetInt64Value(const INT64& value){}

		virtual char GetCharValue()const { return std::numeric_limits<char>::quiet_NaN(); }
		virtual void SetCharValue(char value) {}

		virtual INT8 GetByteValue()const { return std::numeric_limits<INT8>::quiet_NaN(); }
		virtual void SetByteValue(INT8 value){}

		virtual short GetShortValue() const { return std::numeric_limits<short>::quiet_NaN(); }
		virtual void SetShortValue(short) { }

		virtual float GetFloatValue() const { return std::numeric_limits<float>::quiet_NaN(); }
		virtual void SetFloatValue(float) { }

		virtual double GetDoubleValue() const { return std::numeric_limits<double>::quiet_NaN(); }
		virtual void SetDoubleValue(double) { }

		virtual bool GetBoolValue() const { return std::numeric_limits<bool>::quiet_NaN(); }
		virtual void SetBooleanValue(bool) { }

		virtual std::string GetStringValue() const { return std::string(); }
		virtual void SetStringValue(const std::string&) { }
	};
	//**************************************************************************

	template<typename T> static void Assign(T& a, const T& b) { a = b; }
	template<typename T> static void Assign(const T& a, const T& b){}
	template<typename T> static void Assign(T& a, const std::string& b)
	{
		std::istringstream str(b);
		str >> a;
	}
	template<typename T> static void Assign(const T& a, const std::string& b) {}

	//**************************************************************************

#define NumberValueOverride(DataType,FuncName,Value)\
public: \
DataType Get##FuncName##Value()const override {return static_cast<DataType>(Value);}\
void Set##FuncName##Value(const DataType& value)override \
{\
	Assign(Value, static_cast<std::decay<T>::type>(value));\
}

	template<typename T> 
	class NumberValue : public Value
	{
		T mValue;
	protected:
		NumberValue(T value) : mValue(std::forward<T>(value)) {}
	public:
		virtual ~NumberValue() = default;

		operator Value*() { return this; }

		bool IsVoid()const override { return false; }
		//is_arithmetic : 정수 또는 실수형인지 판단
		bool IsNumber()const override { return std::is_arithmetic<T>::value; }
		//std::decay는 데이터의 참조를 풀고 원래 타입을 가져온다
		//std::is_same 는 같은 데이터 타입인지를 비교
		bool IsBool()const override
		{
			return std::is_same<typename std::decay<T>::type, bool>::value;
		}

		NumberValueOverride(int,Int,mValue)
		NumberValueOverride(INT64,Int64,mValue)
		NumberValueOverride(char,Char,mValue)
		NumberValueOverride(INT8,Byte,mValue)
		NumberValueOverride(short,Short,mValue)
		NumberValueOverride(float,Float,mValue)
		NumberValueOverride(double,Double,mValue)
		NumberValueOverride(bool,Bool,mValue)

		std::string GetStringValue()const override
		{
			std::stringstream strStream;
			strStream << mVluae;
			return strStream.str();
		}
		void SetStringValue(const std::string& str)override
		{
			Assign(mValue, str);
		}
	};
	//**************************************************************************
	static void Assign(std::string& a, const std::string& b) { a = b; }
	static void Assign(const std::string& a, const std::string& b) {}
	template<typename T> static void Assign(std::string& str, T value)
	{
		std::stringstream strStream;
		strStream << value;
		str = strStream.str();
	}
	template<typename T> static void Assign(const std::string& str, T value) {}
	template<typename T> static T ToNumber(const std::string& str)
	{
		std::istringstream strStream(str);
		T value;
		strStream >> value;
		return value;
	}
	//**************************************************************************
#define StringValueOverride(DataType,FuncName,Value)\
public:\
DataType Get##FuncName##Value() const override {return ToNumber<DataType>(Value);}\
void Set##FuncName##Value(const DataType& value)override {Assign(Value,value);}

	template<typename T>
	class StringValue : public Value
	{
		T mValue;
	protected:
		StringValue(T value) :mValue(std::forward<T>(value)) {}
		void* ValueDataReference()override { return &mValue; }
	public:
		virtual ~StringValue() = default;
		operator Value*() { return this; }

		bool IsVoid()const override { return false; }
		bool IsString()const override { return true; }

		StringValueOverride(int,Int,mValue)
		StringValueOverride(INT64, Int64,mValue)
		StringValueOverride(char,Char,mValue)
		StringValueOverride(INT8,Byte,mValue)
		StringValueOverride(short,Short,mValue)
		StringValueOverride(float,Float,mValue)
		StringValueOverride(double,Double,mValue)
		StringValueOverride(bool,Bool,mValue)

		std::string GetStringValue()const override {return mValue;}
		void SetStringValue(const str::string& str)override { Assign(mValue, str); }
	};
}
//**************************************************************************
namespace std
{
	template<typename R, typename... P>
	struct is_function<std::function<R(P...)>> : std::true_type {};
	template<typename R, typename... P>
	struct is_function<std::function<R(P...)>&> : std::true_type {};
}
//**************************************************************************
namespace Reflection
{
	template<typename T>
	class ObjectValue : public Value
	{
		T mValue;
	protected:
		ObjectValue(T value) : mValue(std::forward<T>(value)) {}
		void* ValueDataReference()override { return &mValue; }
	public:
		bool IsVoid()const override { return false; }
		bool IsFunction()const override { return std::is_function<T>::value; }

		virtual ~ObjectValue() = default;
		operator Value*() { return this; }
	};
	//**************************************************************************
	struct ErrorValue {};

	typedef Value Void;
	//**************************************************************************
	template<typename T, bool isVoid, bool isArithmetic, bool isString>
	struct ValueTraits { typedef typename T type; };

	template<typename T>
	struct ValueBaseType { typedef ErrorValue type; };

	template<typename T>
	struct ValueBaseType<ValueTraits<T, true, false, false>> { typedef Value type; };

	template<typename T>
	struct ValueBaseType<ValueTraits<T, false, true, false>>
	{
		typedef NumberValue<typename ValueTraits<T, false, true, false>::type> type;
	};

	template<typename T>
	struct ValueBaseType<ValueTraits<T, false, false, true>>
	{
		typedef StringValue<typename ValueTraits<T, false, true, false>::type> type;
	};

	template<typename T>
	struct ValueBaseType<ValueTraits<T, false, false, false>>
	{
		typedef ObjectValue<typename T> type;
	};

	template<typename T>
	struct TypeValueTraits
	{
		typedef typename std::remove_cv<typename std::remove_reference<typename T>::type>::type basetype;
		typedef ValueTraits<T, std::is_void<basetype>::value, std::is_arithmetic<basetype>::value, std::is_same<basetype, std::string>::value> type;
	};

	template<typename T>
	struct TypeValueBaseType
	{
		typedef typename ValueBaseType<typename TypeValueTraits<T>::type>::type type;
	};

	template<typename T>
	class TypeValue : public TypeValueBaseType<T>::type 
	{
		TypeValue() = default;
		TypeValue(T t) : TypeValueBaseType<T>::type(std::forward<T>(t)) {}
	};

	template <typename T>
	class ValueCastFunction
	{
	public:
		static T Cast(Value& value)
		{
			return static_cast<T>(value);
		}
	};

	template<typename T>
	class ValueCastFunction<T&>
	{
	public:
		static T& Cast(Value& value)
		{
			return static_cast<T&>(value);
		}
	};

	template<typename T>
	class ValueCastFunction<T&&>
	{
	public:
		static T Cast(Value& value)
		{
			return static_cast<T>(value);
		}
	};

	template<typename R, typename...P>
	class ValueCastFunction<std::function<R(P...)>>
	{
		static std::function<R(P...)>& Cast(Value& value)
		{
			return value.FunctionObject<R, P...>();
		}
	};
}

