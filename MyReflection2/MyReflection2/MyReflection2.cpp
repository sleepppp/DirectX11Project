
#include <iostream>
#include <Windows.h>
#include <any>
#include <functional>
using namespace std;

#include "Test.h"
#include "Field.h"
#include "Method.h"
using namespace Reflection;

template<typename ...T>
int GetCount(int c, T...args)
{
	return sizeof...(T);
}

template<typename ...T>
std::tuple<T...> GetTuple(T&&... args)
{
	return make_tuple<T...>(std::forward<T>(args)...);
}

int main()
{
	Test test;
	Method* method = new DeducedMethod<void(Test::*)()>("Func", &Test::Func);
	method->Invoke(&test);
	Method* method2 = new DeducedMethod<void(Test::*)(int)>("Print", &Test::Print);
	method2->Invoke(&test, 100);

	delete method2;
	delete method;
}

