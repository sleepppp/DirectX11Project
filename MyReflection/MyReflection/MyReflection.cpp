
#include <iostream>
#include <Windows.h>
#include <malloc.h>
using namespace std;

#include "Class.h"
#include "Field.h"
#include "Test.h"
#include "Method.h"
#include <fstream>

#define GetTypeName(Type) (#Type)

using namespace Reflection;

void Func()
{
	cout << "ad" << endl;
}

int main()
{
	Class<Test> classInfo = GetClass<Test>();
}

