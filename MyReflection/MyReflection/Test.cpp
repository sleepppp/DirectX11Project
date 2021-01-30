#include "Test.h"

namespace Reflection 
{
	template<> void DescribleClass<Test>(Class<Test>& d)
	{
		d.SetDeclaredField("i", &Test::i);
	}
}
