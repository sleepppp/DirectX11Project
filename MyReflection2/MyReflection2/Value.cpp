#include "Value.h"

bool Reflection::Value::IsNum()const
{
	if (mValue.type() == typeid(int))return true;
	if (mValue.type() == typeid(float))return true;
	if (mValue.type() == typeid(bool))return true;
	if (mValue.type() == typeid(short))return true;
	if (mValue.type() == typeid(double))return true;
	if (mValue.type() == typeid(long))return true;
	if (mValue.type() == typeid(long long))return true;
	if (mValue.type() == typeid(unsigned int))return true;
	if (mValue.type() == typeid(unsigned short))return true;
	if (mValue.type() == typeid(unsigned long))return true;
	if (mValue.type() == typeid(unsigned long long))return true;
	return false;
}

bool Reflection::Value::IsBool()const
{
	if (mValue.type() == typeid(bool))return true;
	return false;
}

bool Reflection::Value::IsString() const
{
	if (mValue.type() == typeid(std::string))return true;
	if (mValue.type() == typeid(std::wstring))return true;
	return false;
}

bool Reflection::Value::IsPtr() const
{
	static const char* ptrToken = "*";

	std::string typeName = mValue.type().name();

	if (typeName.find(ptrToken) != std::string::npos)return true;
	return false;
}

bool Reflection::Value::IsFunc() const
{
	static const char* functionToken = "std::function";

	std::string typeName = mValue.type().name();

	if (typeName.find(functionToken) != std::string::npos)return true;
	return false;
}

bool Reflection::Value::IsUnknownType() const
{
	if (IsNum() == false &&
		IsBool() == false &&
		IsString() == false &&
		IsPtr() == false && 
		IsFunc() == false)
		return true;

	return false;
}

bool Reflection::Value::IsClass() const
{
	static const char* functionToken = "class";

	std::string typeName = mValue.type().name();

	if (typeName.find(functionToken) != std::string::npos)return true;
	return false;
}

int Reflection::Value::GetInt()
{
	return std::any_cast<int>(mValue);
}

float Reflection::Value::GetFloat()
{
	return std::any_cast<float>(mValue);
}

bool Reflection::Value::GetBool()
{
	return std::any_cast<bool>(mValue);
}

std::string Reflection::Value::GetString()
{
	return std::any_cast<std::string>(mValue);
}
