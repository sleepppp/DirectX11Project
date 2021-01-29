#pragma once
#include "Value.h"

namespace Reflection
{
	class Field
	{
		std::string mName;
	public:
		explicit Field(const std::string& name) :mName(name) {}
		virtual ~Field() = default;

		std::string GetName()const { return mName; }

		virtual std::shared_ptr<Value> Get(void* instance) = 0;
		virtual void Set(void* instance, Value* value) = 0;

		template<typename T, typename C> T Get(C& instance);
		template<typename T, typename C> void Set(C& instance, T&& value);
	};

	template<typename T,typename C>
	T Field::Get(C& instance)
	{
		return static_cast<T>(*Get(static_cast<void*>(&instance));
	}

	template<typename T,typename C>
	void Field::Set(C& instance, T&& value)
	{
		Set(static_cast<void*>(&instance), &Value(value));
	}
}

