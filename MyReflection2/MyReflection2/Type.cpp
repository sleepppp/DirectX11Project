#include "Type.h"

namespace Reflection
{
	INT64 ClassSharedData::_nextTypeid = 1;

	ClassSharedData::ClassSharedData(const std::string& name)
		:mTypeid(_nextTypeid++),mName(name) {}
}
