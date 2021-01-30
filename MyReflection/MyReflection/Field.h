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

	template<typename T> class DeducedField;

	template<typename T,typename C>
	class DeducedField<T(C::*)> : public Field
	{
		T(C::* mFieldPtr);

		std::shared_ptr<Value> Get(void* instance)override;
		void Set(void* instance, Value* value)override;
	public:
		DeducedField(const std::string& name, T(C::* field))
			:Field(name), mFieldPtr(field) {}

		T Get(C& object)
		{
			return object.*mFieldPtr;
		}
		void Set(C& object, T&& value)
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

	template<typename T,typename C>
	std::shared_ptr<Value> DeducedField<T(C::*)>::Get(void* instance)
	{
		return std::make_shared<TypedValue<T>>(Get(*(static_cast<C*>(instance))));
	}

	template<typename T, typename C>
	void DeducedField<T(C::*)>::Set(void* instance, Value* value)
	{
		Set(*(static_cast<C*>(instance)), static_cast<T>(*value));
	}
}

