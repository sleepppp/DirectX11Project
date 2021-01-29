#pragma once
#include "Value.h"

namespace Reflection
{
	template<typename T>
	class NumberValue : public Value
	{
		T mValue;
	protected:
		void* GetValuePtr() override { return &mValue; }
	public:
		NumberValue(T value):mValue(std::forward<T>(value)){};
		virtual ~NumberValue() = default;
	
		bool IsNum()const override { return std::is_arithmetic<T>::value; }
		bool IsBool()const override{ return std::is_same<typename std::decay<T>::type,bool>::value; }
		bool IsString()const override { return false; }
		bool IsFuncion()const override{ return false; }
	
		int GetInt()const override { return static_cast<int>(mValue); }
		void SetInt(int value)override { mValue = value; }
	
		virtual float GetFloat()const override { return static_cast<float>(mValue); }
		virtual void SetFloat(float value)override { mValue = value; }
	
		virtual bool GetBool()const override { return static_cast<bool>(mValue); }
		virtual void SetBool(bool value)override { mValue = value; }
	
		virtual std::string GetString()const override { std::stringstream s; s << mValue; return s.str(); }
		virtual void SetString(const std::string& value) override { Assign(mValue, value); }
	};
}

