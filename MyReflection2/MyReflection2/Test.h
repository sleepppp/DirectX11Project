#pragma once
#include <iostream>
#include "DefineReflection.h"

class Test;
ReflectionClass(Test)

class Test
{
public:
	int i;

	void Func()
	{
		std::cout << "Func" << std::endl;
	}

	void Print(int i, int j)
	{
		std::cout << i << std::endl;
		std::cout << j << std::endl;
	}
};



