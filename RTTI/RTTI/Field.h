#pragma once
#include "Value.h"

namespace Reflection
{
	class Field
	{
		std::string mName;
	public:
		Field(const std::string& name)
			:mName(name) {}
		virtual ~Field() = default;

		std::string GetName()const { return mName; }

		template<typename T, typename C> T Get(C&  receiver);
		template<typename T, typename C> void Set(C& receiver, T&& t);

		virtual std::shared_ptr<Value> Get(void* receiver) = 0;
		virtual void Set(void* receiver, Value* value) = 0;
	};

	template<typename T, typename C>
	T Field::Get(C& receiver)
	{
		return static_cast<T>(*Get(static_cast<void*>(&receiver)));
	}

	template<typename T, typename C>
	void Field::Set(C& receiver, T&& value)
	{
		Set(static_cast<void*>(&receiver), static_cast<Value*>(TypeValue<T>(value)));
	}

	template<typename>class DeducedField;

	template<typename T, typename C> 
	class DeducedField <T(C::*)> : public Field
	{
		T(C::* mField);

		std::shared_ptr<Value> Get(void* receiver)override;
		void Set(void* receiver, Value* value)override;
	public:
		DeducedField(const std::string& name, T(C::* field))
			:Field(name), mField(field) {}

		T Get(C& object)
		{
			return object.*mField;
		}
		void Set(C& object, T&& value)
		{
			(object.*mField) = value;
		}
	};

	template<typename T,typename C> 
	std::shared_ptr<Value> DeducedField<T(C::*)>::Get(void* receiver)
	{
		return std::make_shared<TypeValue<T>>(Get(*(static_cast<C*>(receiver))));
	}

	template<typename T, typename C>
	void DeducedField<T(C::*)>::Set(void* receiver, Value* value)
	{
		Set(*(static_cast<C*>(receiver)), (static_cast<T>(*value));
	}
}

