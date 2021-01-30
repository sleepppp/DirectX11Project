
#include <iostream>
#include <Windows.h>
#include <malloc.h>
using namespace std;

#include "Class.h"
#include "Test.h"


int main()
{
	Reflection::Class<Test> testClass = Reflection::GetClass<Test>();
	//shared_ptr<Test> ptr = testClass.NewInstance();
	//
	//ptr.get()->i = 10;
	//cout << ptr.get()->i << endl;

	vector<shared_ptr<Reflection::Field>> fields = testClass.GetDeclaredFields();
	for (int i = 0; i < fields.size(); ++i)
	{
		cout << fields[i].get()->GetName().c_str() << endl;
	}
}

