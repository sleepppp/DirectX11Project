#pragma once
#include <string>
#include <any>
#include <stdint.h>

namespace Reflection
{
	class Value
	{
		std::any mValue;
	public:
		Value() = default;
		template<typename T> 
		Value( T value) 
		{ 
			mValue = value; 
		}
		virtual ~Value() = default;

		bool IsNum()const;
		bool IsBool()const;
		bool IsString()const;
		bool IsPtr()const;
		bool IsFunc()const;
		bool IsUnknownType()const;

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

