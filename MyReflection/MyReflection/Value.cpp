#include "Value.h"
#include "Class.h"


Reflection::ClassUnknown& Reflection::Value::GetClass()const
{
	return Reflection::GetClass<void>();
}