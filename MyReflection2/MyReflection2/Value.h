#pragma once
#include <string>
#include <any>

namespace Reflection
{
	class Value
	{
		std::any mValue;
	public:
		Value() = default;

		template<typename T> 
		Value(T value) 
		{ 
			mValue = value; 
		}
		Value(const Value& origin)
		{
			mValue = origin.mValue;
		}

		Value& operator=(const Value& origin)
		{
			mValue = origin.mValue;
			return *this;
		}

		virtual ~Value()
		{
			int a = 3;
		}

		bool IsNum()const;
		bool IsBool()const;
		bool IsString()const;
		bool IsPtr()const;
		bool IsFunc()const;
		bool IsUnknownType()const;
		bool IsClass()const;

		int GetInt();
		float GetFloat();
		bool GetBool();
		std::string GetString();

		template<typename T> inline T Get();
		template<typename T> inline void Set(T t);
	};

	template<typename T>
	inline T Value::Get()
	{
		return std::any_cast<T>(mValue);
	}

	template<typename T>
	inline void Value::Set(T value)
	{
		mValue = value;
	}

}

