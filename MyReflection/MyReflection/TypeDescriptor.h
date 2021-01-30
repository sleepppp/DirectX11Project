#pragma once
#include <string>
#include <sstream>
#include <memory>
#include <string>
#include <vector>
#include <stdint.h>
#include <assert.h>
#include <functional>

namespace Reflection
{
	template<typename T>
	class TypeDescriptor
	{
	public:
		static std::string Descriptor() { return "<unknown>"; }
	};

	static std::string Concat(std::initializer_list<std::string>&& strings)
	{
		static const std::string space(" ");
		std::string result("<");
		for (auto& string : strings)
		{
			result += (string + space);
		}
		return result;
	}

	template<typename... A>
	class VariadicTypeDescriptor
	{
	public:
		static std::string Descriptor() {return Concat({ TypeDescriptor<A>::Descriptor()... });}
	};

	template<typename>class Class;

	template<typename T>
	void DescribleClass(Class<T>&)
	{

	}
}