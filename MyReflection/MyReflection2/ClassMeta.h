#pragma once
#include <vector>
#include "FieldMeta.h"
#include "FuncMeta.h"
class ClassMeta
{
	std::vector<FieldMeta> mFieldList;
	std::vector<FuncMeta> mFuncList;
};