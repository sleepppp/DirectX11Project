#pragma once
#include <sstream>
#include <memory>
#include <string>
#include <vector>
#include <stdint.h>

/*
데이터를 담을 클래스 => 변수가 담길 수도 있고 함수가 담길 수도 있다.
*/

namespace Reflection
{
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
	};
	
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
}
