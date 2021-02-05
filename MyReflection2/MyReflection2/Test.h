#pragma once
#include <iostream>
class Test
{
public:
	int i;

	void Func()
	{
		std::cout << "Func" << std::endl;
	}

	void Print(int i)
	{
		std::cout << i << std::endl;
	}
};

