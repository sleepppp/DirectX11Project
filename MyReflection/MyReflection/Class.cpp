#include "Class.h"

namespace Reflection
{
	int64_t ClassSharedData::_nextTypeid = 1;

	ClassSharedData::ClassSharedData()
		:mTypeid(_nextTypeid++) {}

	ClassSharedData& ClassUnknown::GetClassSharedData(const std::string& key)
	{
		static std::unordered_map<std::string, std::unique_ptr<ClassSharedData>> dataList;
		if (dataList.count(key) == 0)
			dataList[key] = std::make_unique<ClassSharedData>();
		return *dataList[key];
	}

	std::map<void*, ClassUnknown*>& ClassUnknown::Instances()
	{
		static std::map<void*, ClassUnknown*> instanceList;
		return instanceList;
	}
}