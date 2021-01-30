#pragma once
#include "Method.h"

namespace Reflection
{
	template<typename>
	class MethodDescriptor;

	std::string MethodDescriptorFunc(const std::string& classDescriptor, const std::string& name,
		size_t numParameters, const std::string parameterTypeDescriptors[]);

	template<typename R>
	class MethodDescriptor<R(*)()>
	{
	public:
		static std::string Descriptor(const std::string& className, const std::string& methodName)
		{
			return MethodDescriptorFunc(className, methodName, 0, nullptr);
		}
	};

	template<typename R,typename... P>
	class MethodDescriptor<R(*)(P...)>
	{
	public:
		static std::string Descriptor(const std::string& className, const std::string& methodName)
		{
			std::string descriptors[] = { TypeDescriptor<P>::Descriptor()... };
			return MethodDescriptor(className, methodName, sizeof...(P), descriptors);
		}
	};

	template<typename R,typename C>
	class MethodDescriptor<R(C::*)()>
	{
	public:
		static std::string Descriptor(const std::string& className, const std::string& methodName)
		{
			return MethodDescriptorFunc(className, methodName, 0, nullptr);
		}
	};

	template<typename R,typename C,typename...P>
	class MethodDescriptor<R(C::*)(P...)>
	{
	public:
		static std::string Descriptor(const std::string& className, const std::string& methodName)
		{
			std::string descriptor[] = { TypeDescriptor<P>::Descriptor()... };
			return MethodDescriptorFunc(className, methodName, sizeof...(P), descriptor);
		}
	};

}

