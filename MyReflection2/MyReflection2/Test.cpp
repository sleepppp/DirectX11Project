#include "Test.h"

namespace Reflection
{
	template<> void DescribleType<Test>(Type<Test>& type)
	{
		type.SetMethod("Func", &Test::Func);
		type.SetMethod("Print", &Test::Print);
		type.SetField("i", &Test::i);
	}
}