#include <iostream>
#include <Windows.h>
#include <any>
#include <functional>
using namespace std;

#include "Test.h"
#include "Field.h"
#include "Method.h"
using namespace Reflection;

int main()
{
	Type<Test> type = Reflection::GetType<Test>();
	vector<shared_ptr<Method>> methods = type.GetMethods();
	vector<shared_ptr<Field>> fields = type.GetFields();
	Test* t = type.CreateInstance();

	cout << "ClassName : " << type.GetName().c_str() << endl;

	for (int i = 0; i < methods.size(); ++i)
	{
		cout << "Method : " <<methods[i].get()->GetName() << endl;
	}

	for (int i = 0; i < fields.size(); ++i)
	{
		cout << "Field : " << fields[i].get()->GetName() << endl;
		fields[i].get()->SetValue(t, 100);
	}

	cout << "Test::i : " << t->i << endl;

	delete t;
}

