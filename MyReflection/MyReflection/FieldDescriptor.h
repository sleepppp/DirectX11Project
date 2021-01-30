#pragma once
#include "TypeDescriptor.h"

namespace Reflection
{
	std::string FieldDescriptorFunc(const std::string& classDescriptor, const std::string& name);

	template<typename>
	class FieldDescriptor;

	template<typename T, typename C>
	class FieldDescriptor<T(C::*)>
	{
	public:
		static std::string Descriptor(const std::string& className, const std::string& fieldName)
		{
			return FieldDescriptorFunc(className, fieldName);
		}
	};
}