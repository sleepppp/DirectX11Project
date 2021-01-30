#pragma once
#include "FieldDescriptor.h"
#include "Value.h"

namespace Reflection
{
	class Field
	{
		std::string mName;
	public:
		Field(const std::string& name) :mName(name) {}
		virtual ~Field() = default;

		std::string GetName()const { return mName; }

		template<typename T, typename C> T Get(C& instance);
		template<typename T, typename C> void Set(C& instance, T&& value);
	private:
		virtual std::shared_ptr<Value> Get(void* instance) = 0;
		virtual void Set(void* instance, Value* value) = 0;
	};

	template<typename DataType> class DeducedField;

	template<typename DataType,typename Class>
	class DeducedField<DataType(Class::*)> : public Field
	{
		DataType(Class::* mFieldPtr);

		std::shared_ptr<Value> Get(void* instance)override;
		void Set(void* instance, Value* value)override;
	public:
		DeducedField(const std::string& name, DataType(Class::* field))
			:Field(name), mFieldPtr(field) {}

		DataType Get(Class& object)
		{
			return object.*mFieldPtr;
		}
		void Set(Class& object, DataType&& value)
		{
			(object.*mFieldPtr) = value;
		}
	};

	template<typename T,typename C>
	T Field::Get(C& instance)
	{
		T value = Get(static_cast<void*>(&instance));
		return static_cast<T>(*value);
	}

	template<typename T, typename C>
	void Field::Set(C& instance, T&& value)
	{
		Set(static_cast<void*>(&instance), static_cast<Value*>(TypedValue<T>(value)));
	}

	template<typename DataType,typename Class>
	std::shared_ptr<Value> DeducedField<DataType(Class::*)>::Get(void* instance)
	{
		return std::make_shared<TypedValue<DataType>>(Get(*(static_cast<Class*>(instance))));
	}

	template<typename DataType, typename Class>
	void DeducedField<DataType(Class::*)>::Set(void* instance, Value* value)
	{
		Set(*(static_cast<Class*>(instance)), static_cast<DataType>(*value));
	}
}

