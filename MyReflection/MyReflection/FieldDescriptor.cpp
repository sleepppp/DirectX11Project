#include "FieldDescriptor.h"

namespace Reflection
{
	std::string FieldDescriptorFunc(const std::string& classDescriptor, const std::string& name)
	{
		std::stringstream descriptor;
		if (!classDescriptor.empty())
			descriptor << classDescriptor << "::";
		descriptor << name;
		return descriptor.str();
	}
}