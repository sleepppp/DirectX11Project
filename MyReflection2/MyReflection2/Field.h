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

		virtual bool IsNum()const = 0;
		virtual bool IsBool()const = 0;
		virtual bool IsString()const =0 ;
		virtual bool IsPtr()const = 0;
		virtual bool IsFunc()const = 0;
		virtual bool IsUnknownType()const = 0;
		virtual bool IsClass()const = 0;
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
		static Value _typeValue;
		T(C::* mFieldPtr);
	public:
		DeducedField(const std::string& name, T(C::* field))
			:Field(name), mFieldPtr(field)
		{
			_typeValue = T();
		}
	public:
		T GetValue(C* instance)
		{
			return instance->*mFieldPtr;
		}

		void SetValue(C* instance, T&& value)
		{
			instance->*mFieldPtr = value;
		}

		bool IsNum()const override;
		bool IsBool()const override;;
		bool IsString()const override;
		bool IsPtr()const override;
		bool IsFunc()const override;
		bool IsUnknownType()const override;
		bool IsClass()const override;
	protected:
		std::shared_ptr<Value> Get(void* instance)override;
		void Set(void* instance, Value* value)override;
	};

	template<typename T, typename C>
	Value DeducedField<T(C::*)>::_typeValue;

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

	template<typename T, typename C>
	inline bool DeducedField<T(C::*)>::IsNum() const
	{
		return _typeValue.IsNum();
	}

	template<typename T, typename C>
	inline bool DeducedField<T(C::*)>::IsBool() const
	{
		return _typeValue.IsBool();
	}

	template<typename T, typename C>
	inline bool DeducedField<T(C::*)>::IsString() const
	{
		return _typeValue.IsString();
	}

	template<typename T, typename C>
	inline bool DeducedField<T(C::*)>::IsPtr() const
	{
		return _typeValue.IsPtr();
	}

	template<typename T, typename C>
	inline bool DeducedField<T(C::*)>::IsFunc() const
	{
		return _typeValue.IsFunc();
	}

	template<typename T, typename C>
	inline bool DeducedField<T(C::*)>::IsUnknownType() const
	{
		return _typeValue.IsUnknownType();
	}

	template<typename T, typename C>
	inline bool DeducedField<T(C::*)>::IsClass() const
	{
		return _typeValue.IsClass();
	}

}

