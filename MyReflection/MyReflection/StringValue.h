#pragma once
#include "Value.h"

namespace Reflection
{
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
	};

}