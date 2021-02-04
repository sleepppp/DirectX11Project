#pragma once
#include "Value.h"
namespace Reflection
{
	class Field
	{
		std::string mName;
	public:
		Field(const std::string& name) :mName(name) {}
		
		std::string GetName()const { return mName; }

		template<typename T, typename C> T GetValue(C* instance);
		template<typename T, typename C> void SetValue(C* instance, T&& value);		
	protected:
		virtual std::shared_ptr<Value> Get(void* instance) = 0;
		virtual void Set(void* instance, Value* value) = 0;
	};

	template<typename T, typename C>
	inline T Field::GetValue(C * instance)
	{
		std::shared_ptr<Value> result = Get(static_cast<void*>(instance));
		return result.get()->Get<T>();
	}

	template<typename T, typename C>
	inline void Field::SetValue(C * instance, T && value)
	{
		Value reflectionValue;
		reflectionValue.Set(value);
		Set((void*)instance, &reflectionValue);
	}

	template<typename T> class DeducedField;

	template<typename T, typename C>
	class DeducedField<T(C::*)> final : public Field
	{
		T(C::* mFieldPtr);
	public:
		DeducedField(const std::string& name, T(C::* field))
			:Field(name), mFieldPtr(field) {}
	public:
		T GetValue(C* instance)
		{
			return instance->*mFieldPtr;
		}

		void SetValue(C* instance, T&& value)
		{
			instance->*mFieldPtr = value;
		}
	protected:
		std::shared_ptr<Value> Get(void* instance)override;
		void Set(void* instance, Value* value)override;
	};

	template<typename T, typename C>
	inline std::shared_ptr<Value> DeducedField<T(C::*)>::Get(void * instance)
	{
		std::shared_ptr<Value> result(new Value());
		C* ptrC = static_cast<C*>(instance);
		result.get()->Set<T>(ptrC->*mFieldPtr);
		return result;
	}

	template<typename T, typename C>
	inline void DeducedField<T(C::*)>::Set(void * instance, Value * value)
	{
		SetValue(static_cast<C*>(instance), value->Get<T>());
	}

}

