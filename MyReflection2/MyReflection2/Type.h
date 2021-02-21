#pragma once
#include <Windows.h>
#include <set>
#include <map>
#include <memory>
#include "Method.h"
#include "Field.h"
#include "Descriptor.h"

namespace Reflection
{
	class ClassSharedData
	{
		template<typename> friend class Type;
	private:
		static INT64 _nextTypeid;

		INT64 mTypeid;

		std::map<std::string, std::shared_ptr<Method>> mMethods;
		std::map<std::string, std::shared_ptr<Field>> mFields;
		//std::set<void*> mInstances;
		std::string mName;
		ClassSharedData(const std::string& name);
		typedef std::set<void*>::iterator InstanceIter;
	public:
		static ClassSharedData* CreateNew(const std::string& name)
		{
			return new ClassSharedData(name);
		}
	};

	template<typename T>
	class Type
	{
		static ClassSharedData* _sharedData;
	public:
		Type()
		{
			if (_sharedData == nullptr)
			{
				_sharedData = ClassSharedData::CreateNew(TypeDescriptor<T>::Descriptor());
			}
		}

		template<typename M>
		void SetMethod(const std::string&, M);
		template<typename F>
		void SetField(const std::string&, F);

		std::string GetName()
		{
			if (_sharedData)
				return _sharedData->mName;
		}
		//INT64 GetTypeid() { if (_shareData)return _sharedData->mTypeid; return 0; }

		std::shared_ptr<Method> GetMethod(const std::string&);
		std::shared_ptr<Field> GetField(const std::string&);
		std::vector<std::shared_ptr<Method>> GetMethods();
		std::vector<std::shared_ptr<Field>> GetFields();

		void DeleteInstance(T* instance);
		template<typename...P>
		T* CreateInstance(P...);
	};

	template<typename T>
	ClassSharedData* Type<T>::_sharedData = nullptr;

	template<typename T> template<typename M>
	inline void Type<T>::SetMethod(const std::string & methodName, M method)
	{
		std::string descriptor = MethodDescriptor<M>::Descriptor(GetName(), methodName);
		_sharedData->mMethods[descriptor] = std::make_shared<DeducedMethod<M>>(methodName, method);
	}

	template<typename T> template<typename F>
	inline void Type<T>::SetField(const std::string & fieldName, F field)
	{
		std::string descriptor = FieldDescriptor<F>::Descriptor(GetName(), fieldName);
		_sharedData->mFields[descriptor] = std::make_shared<DeducedField<F>>(fieldName, field);
	}

	template<typename T>
	inline std::shared_ptr<Method> Type<T>::GetMethod(const std::string & description)
	{
		return _sharedData->mMethods[description];
	}

	template<typename T>
	inline std::shared_ptr<Field> Type<T>::GetField(const std::string & description)
	{
		return _sharedData->mFields[description];
	}

	template<typename T>
	inline std::vector<std::shared_ptr<Method>> Type<T>::GetMethods()
	{
		std::vector<std::shared_ptr<Method>> result;
		for (auto& item : _sharedData->mMethods)
		{
			result.push_back(item.second);
		}
		return result;
	}

	template<typename T>
	inline std::vector<std::shared_ptr<Field>> Type<T>::GetFields()
	{
		std::vector<std::shared_ptr<Field>> result;
		for (auto& item : _sharedData->mFields)
		{
			result.push_back(item.second);
		}
		return result;
	}

	template<typename T>
	inline void Type<T>::DeleteInstance(T * instance)
	{
		delete instance;
	}

	template<typename T> template<typename ...P>
	inline T * Type<T>::CreateInstance(P ...args)
	{
		T* instance = new T(args...);
		return instance;
	}
}