#include "Descriptor.h"

#include <sstream>
namespace Reflection
{
	std::string Reflection::GetFieldDescript(const std::string & className, const std::string & name)
	{
		std::stringstream str;
		if (className.empty() == false)
			str << className << "::";
		str << name;
		return str.str();
	}

	std::string MethodDescriptorFunc(const std::string& classDescriptor, const std::string& name,
		size_t numParameters, const std::string parameterTypeDescriptors[])
	{
		std::stringstream descriptor;
		if (!classDescriptor.empty())
			descriptor << classDescriptor << "::";
		descriptor << name << "(";
		for (size_t index = 0; index < numParameters; ++index)
			descriptor << parameterTypeDescriptors[index] << ((index < numParameters - 1) ? "," : ")");
		return descriptor.str();
	}
}
