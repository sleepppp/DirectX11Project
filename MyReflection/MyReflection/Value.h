#pragma once
#include "TypeDescriptor.h"

namespace std
{
	template<typename R, typename...P>
	struct is_function<std::function<R(P...)>> : std::true_type {};

	template<typename R, typename...P>
	struct is_function < std::function<R(P...)&>> : std::true_type {};
}

namespace Reflection
{
	class ClassUnknown;

	class Value
	{
	protected:
		virtual void* GetValuePtr() { return nullptr; }
	public:
		Value() = default;
		virtual ~Value() = default;

		virtual bool IsNum()const { return true; }
		virtual bool IsBool()const { return false; }
		virtual bool IsString()const { return false; }
		virtual bool IsFuncion()const { return false; }

		//Nan값으로 반환
		virtual int GetInt()const { return std::numeric_limits<int>::quiet_NaN(); }
		virtual void SetInt(int) {}

		virtual float GetFloat()const { return std::numeric_limits<float>::quiet_NaN(); }
		virtual void SetFloat(float) {}

		virtual bool GetBool()const { return std::numeric_limits<bool>::quiet_NaN(); }
		virtual void SetBool(bool) {}

		virtual std::string GetString()const { return std::string(); }
		virtual void SetString(const std::string&) {}

		template<typename T> operator T& ();
		template<typename T>operator T* ();

		template<typename R, typename...P>
		std::function<R(P...)>& GetFunction();

		operator int() { return GetInt(); }
		operator float() { return GetFloat(); }
		operator bool() { return GetBool(); }
		operator std::string() { return GetString(); }

		virtual ClassUnknown& GetClass()const;
	};

	template<typename T> Class<T>& GetClass();

	template<typename T>
	Value::operator T& ()
	{
		if (Reflection::GetClass<T>().IsAssignableFrom(GetClass()) == false)
		{
			assert(false);
		}
		return *(reinterpret_cast<T*>(GetValuePtr()));
	}

	template<typename T>
	Value::operator T* ()
	{
		if (Reflection::GetClass<T>().IsAssignableFrom(GetClass()) == false)
		{
			assert(false);
		}
		return reinterpret_cast<T*>(GetValuePtr());
	}

	template<typename R, typename...P>
	std::function<R(P...)>& Value::GetFunction()
	{
		typedef std::function<R(P...)> T;
		if (IsFuncion() == false ||
			Reflection::GetClass<T>().IsAssignableFrom(GetClass()) == false)
		{
			assert(false);
		}

		return *(reinterpret_cast<T*>(GetValuePtr()));
	}

	template<typename T>
	static T ToNumber(const std::string& str)
	{
		std::istringstream stream(str);
		T value;
		stream >> value;
		return value;
	}

	template<typename T>
	static void Assign(T& a, const T& b) { a = b; }

	template<typename T>
	static void Assign(const T& a, const T& b) {}

	template<typename T>
	static void Assign(T& a, const std::string& b)
	{
		std::istringstream stream(b);
		stream >> a;
	}

	template<typename T>
	static void Assign(const T& a, const std::string& b) {}

	template<typename T>
	class NumberValue : public Value
	{
		T mValue;
	protected:
		void* GetValuePtr() override { return &mValue; }
	public:
		NumberValue(T value) :mValue(std::forward<T>(value)) {};
		virtual ~NumberValue() = default;

		bool IsNum()const override { return std::is_arithmetic<T>::value; }
		bool IsBool()const override { return std::is_same<typename std::decay<T>::type, bool>::value; }
		bool IsString()const override { return false; }
		bool IsFuncion()const override { return false; }

		int GetInt()const override { return static_cast<int>(mValue); }
		void SetInt(int value)override { mValue = value; }

		virtual float GetFloat()const override { return static_cast<float>(mValue); }
		virtual void SetFloat(float value)override { mValue = value; }

		virtual bool GetBool()const override { return static_cast<bool>(mValue); }
		virtual void SetBool(bool value)override { mValue = value; }

		virtual std::string GetString()const override { std::stringstream s; s << mValue; return s.str(); }
		virtual void SetString(const std::string& value) override { Assign(mValue, value); }

		ClassUnknown& GetClass()const override { return Reflection::GetClass<T>(); }
	};

	template<typename T>
	class StringValue : public Value
	{
		T mValue;
	protected:
		void* GetValuePtr() override { return &mValue; }
	public:
		StringValue(std::string str) : mValue(std::forward<T>(str)) {}
		virtual ~StringValue() = default;

		bool IsInt()const override { return false; }
		bool IsString()const override { return true; }

		int GetInt()const override { return ToNumber<int>(mValue); }
		void SetInt(int value)override { Assign(mValue, value); }

		virtual float GetFloat()const override { return ToNumber<float>(mValue); }
		virtual void SetFloat(float value)override { Assign(mValue, value); }

		virtual bool GetBool()const override { return ToNumber<bool>(mValue); }
		virtual void SetBool(bool value)override { Assign(mValue, value); }

		virtual std::string GetString()const override { return mValue; }
		virtual void SetString(const std::string& value) override { Assign(mValue, value); }

		operator Value& () { return this; }

		ClassUnknown& GetClass()const override { return Reflection::GetClass<std::string>(); }
	};

	template<typename T>
	class ObjectValue : public Value
	{
		T mValue;
	public:
		ObjectValue(T value) : mValue(std::forward<T>(value)) {}
		virtual ~ObjectValue() = default;

		bool IsNumber()override { return false; }

		void* GetValuePtr()override { return &mValue; }

		operator Value* () { return this; }

		ClassUnknown& GetClass()const override { return Reflection::GetClass<T>(); }
	};

	struct ErrorValue {};

	typedef Value Void;

	template<typename T, bool IsVoid, bool IsArithmetic, bool IsString>
	struct ValueTraits
	{
		typedef typename T Type;
	};

	template<typename T>
	struct ValueBaseType
	{
		typedef ErrorValue Type;
	};

	template<typename T>
	struct ValueBaseType<ValueTraits<T, true, false, false>>
	{
		typedef Value Type;
	};

	//{{ Number Value ~
	template<typename T>
	struct ValueBaseType<ValueTraits<T, false, true, false>>
	{
		typedef NumberValue<typename ValueTraits<T, false, true, false>::Type> Type;
	};
	// }}

	// {{ StringValue ~
	template<typename T>
	struct ValueBaseType<ValueTraits<T, false, false, true>>
	{
		typedef StringValue<typename ValueTraits<T, false, false, false>::Type> Type;
	};
	// }}

	template<typename T>
	struct ValueBaseType<ValueTraits<T, false, false, false>>
	{
		typedef ObjectValue<typename T> Type;
	};

	template<typename T>
	struct TypeValueTraits
	{
		typedef typename std::remove_cv<typename std::remove_reference<typename T>::type>::type BaseType;
		typedef ValueTraits
			<T,
			std::is_void<BaseType>::value,
			std::is_arithmetic<BaseType>::value,
			std::is_same<BaseType, std::string>::value> Type;
	};

	template<typename T>
	struct TypeValueBaseType
	{
		typedef typename ValueBaseType<typename TypeValueTraits<T>::Type>::Type Type;
	};

	template<typename T>
	class TypedValue : public TypeValueBaseType<T>::Type
	{
	public:
		TypedValue() = default;
		TypedValue(T t) : TypeValueBaseType<T>::Type(std::forward<T>(t)) {}
	};

	template<typename T>
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
	public:
		static std::function<R(P...)>& Cast(Value& value)
		{
			return value.GetFunction<R, P...>();
		}
	};

}
