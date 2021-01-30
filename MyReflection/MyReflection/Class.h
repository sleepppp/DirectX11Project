#pragma once
#include "Field.h"
#include "MethodDescriptor.h"

#include <map>
#include <unordered_map>
#include <set>

namespace Reflection
{
	template<typename> class Class;

	enum LeakPtrTag {LeakPtr};

	template<typename T, bool is_pointer, bool is_reference> 
	struct Remove_const_if_helper { typedef T Type; };

	template<typename T> 
	struct Remove_const_if_helper<T, false, false> { typedef typename std::remove_cv<T>::type Type; };

	template<typename T> 
	struct Remove_const_if : public Remove_const_if_helper<T, std::is_pointer<T>::value, std::is_reference<T>::value> { };

	class ClassSharedData
	{
		template<typename> friend class ClassBase;
	private:
		static int64_t _nextTypeid;

		int64_t mTypeid;

		std::map<std::string, std::shared_ptr<Method>> mMethods;
		std::map<std::string, std::shared_ptr<Field>> mFields;
		std::set<void*> mInstances;
	public:
		ClassSharedData();
	};

	class ClassUnknown
	{
		static std::map<void*, ClassUnknown*>& Instances();
	protected:
		virtual int64_t GetTypeid()const = 0;
		virtual std::shared_ptr<Method> GetDeclaredMethodFromDescription(const std::string&)const = 0;
		virtual std::shared_ptr<Field> GetDeclaredFieldFromDescription(const std::string&)const = 0;

		void TrackInstance(void*);
		void ForgetInstance(void*);

		static ClassSharedData& GetClassSharedData(const std::string& key);
	public:
		static ClassUnknown* GetClass(void*);
		static ClassUnknown* GetClass(const std::shared_ptr<void>& ptr) { return GetClass(ptr.get()); }

		virtual std::string GetName()const = 0;

		std::shared_ptr<Method> GetDeclaredMethod(const std::string&)const;
		template<typename...P>
		std::shared_ptr<Method> GetDeclaredMethod(const std::string&, const Class<P>&...)const;

		virtual std::vector<std::shared_ptr<Method>> GetDeclaredMethods()const = 0;

		std::shared_ptr<Field> GetDeclaredField(const std::string&)const;
		virtual std::vector<std::shared_ptr<Field>> GetDeclaredFields()const = 0;

		bool IsAssignableFrom(ClassUnknown&)const;
		virtual bool IsInstance(void*)const;
		bool IsInstance(const std::shared_ptr<void>& ptr)const { return IsInstance(ptr.get()); }
	};

	inline ClassUnknown* ClassUnknown::GetClass(void* instance)
	{
		if (instance == nullptr || Instances().count(instance) == 0)
			return nullptr;

		return Instances()[instance];
	}

	inline std::shared_ptr<Method> ClassUnknown::GetDeclaredMethod(const std::string& methodName)const
	{
		std::string descriptor = MethodDescriptorFunc(GetName(), methodName, 0, nullptr);
		return GetDeclaredMethodFromDescription(descriptor);
	}

	template<typename...P>
	std::shared_ptr<Method> ClassUnknown::GetDeclaredMethod(const std::string& methodName,
		const Class<P>&... parameterTypes)const
	{
		std::string parameterTypeDescriptors[] = { parameterTypes.GetName()... };
		std::string descriptor = MethodDescriptorFunc(GetName(), methodName, sizeof...(P), parameterTypeDescriptors);
		return GetDeclaredMethodFromDescription(descriptor);
	}

	inline std::shared_ptr<Field> ClassUnknown::GetDeclaredField(const std::string& fieldName)const
	{
		std::string descriptor = FieldDescriptorFunc(GetName(), fieldName);
		return GetDeclaredFieldFromDescription(descriptor);
	}

	inline bool ClassUnknown::IsAssignableFrom(ClassUnknown& other)const
	{
		if (GetTypeid() == other.GetTypeid())
			return true;
		return false;
	}

	inline bool ClassUnknown::IsInstance(void* ptr)const
	{
		return Instances().count(ptr) > 0;
	}

	inline void ClassUnknown::TrackInstance(void* instance)
	{
		Instances()[instance] = this;
	}

	inline void ClassUnknown::ForgetInstance(void* instance)
	{
		Instances().erase(instance);
	}

	template<typename T>
	class ClassBase : public ClassUnknown
	{
		static ClassSharedData* _sharedData;
	protected:
		ClassBase()
		{
			if (_sharedData == nullptr)
			{
				_sharedData = &ClassUnknown::GetClassSharedData(TypeDescriptor<T>::Descriptor());
			}
		}

		template<typename M> void SetDeclaredMethod(const std::string&, M);
		template<typename F> void SetDeclaredField(const std::string&, F);

		int64_t GetTypeid()const override { return _sharedData->mTypeid; }
		std::shared_ptr<Method> GetDeclaredMethodFromDescription(const std::string&)const override;
		std::shared_ptr<Field> GetDeclaredFieldFromDescription(const std::string&)const override;

		void DeleteInstance(T*);
	public:
		std::vector<std::shared_ptr<Method>> GetDeclaredMethods()const override;
		std::vector<std::shared_ptr<Field>> GetDeclaredFields()const override;

		template<typename... Args> T* NewInstance(LeakPtrTag, Args&&... args);
		template<typename... Args> std::shared_ptr<T> NewInstance(Args&&... args);
		bool IsInstance(void*)const override;
		bool IsInstance(const std::shared_ptr<void>& ptr)const { return IsInstance(ptr.get()); }
	};

	template<typename T>
	ClassSharedData* ClassBase<T>::_sharedData;

	template<typename T>
	std::vector<std::shared_ptr<Method>> ClassBase<T>::GetDeclaredMethods()const
	{
		std::vector<std::shared_ptr<Method>> methods;
		for (auto& method : _sharedData->mMethods)
		{
			methods.push_back(method.second);
		}
		return methods;
	}

	template<typename T>
	std::vector<std::shared_ptr<Field>> ClassBase<T>::GetDeclaredFields()const
	{
		std::vector<std::shared_ptr<Field>> fields;
		for (auto& field : _sharedData->mFields)
		{
			fields.push_back(field.second);
		}
		return fields;
	}

	template<typename T> template<typename...Args>
	T* ClassBase<T>::NewInstance(LeakPtrTag, Args&&... args)
	{
		T* instance = new T(args...);
		ClassUnknown::TrackInstance(instance);
		_sharedData->mInstances.insert(instance);
		return instance;
	}

	template<typename T> template<typename... Args>
	std::shared_ptr<T> ClassBase<T>::NewInstance(Args&&...args)
	{
		T* instance = NewInstance(LeakPtr, std::forward(args)...);

		struct Deleter
		{
			std::function<void(T*)> mDeleter;

			void operator()(T* ptr)const
			{
				mDeleter(ptr);
			}
		};

		return std::shared_ptr<T>(instance, Deleter({ [=](T* ptr) {DeleteInstance(ptr); } }));
	}

	template<typename T>
	void ClassBase<T>::DeleteInstance(T* instance)
	{
		ClassUnknown::ForgetInstance(instance);
		_sharedData->mInstances.erase(instance);
		delete instance;
	}

	template<typename T>
	bool ClassBase<T>::IsInstance(void* ptr)const
	{
		return _sharedData->mInstances.count(ptr) > 0;
	}

	template<typename T> template<typename M> 
	void ClassBase<T>::SetDeclaredMethod(const std::string& methodName, M method)
	{
		std::string descriptor = MethodDescriptor<M>::Descriptor(GetName(), methodName);
		_sharedData->mMethods[descriptor] = std::make_shared<DeducedMethod<M>>(methodName, method);
	}

	template<typename T> template<typename F>
	void ClassBase<T>::SetDeclaredField(const std::string& fieldName,F field)
	{
		std::string descriptor = FieldDescriptor<F>::Descriptor(GetName(), fieldName);
		_sharedData->mFields[descriptor] = std::make_shared<DeducedField<F>>(fieldName, field);
	}

	template<typename T>
	std::shared_ptr<Method> ClassBase<T>::GetDeclaredMethodFromDescription(const std::string& descriptor)const
	{
		return _sharedData->mMethods[descriptor];
	}

	template<typename T>
	std::shared_ptr<Field> ClassBase<T>::GetDeclaredFieldFromDescription(const std::string& descriptor)const
	{
		return _sharedData->mFields[descriptor];
	}

	template<typename T>
	class Class : public ClassBase<typename TypeValueTraits<T>::BaseType>
	{
		friend T;
		friend void DescribleClass<T>(Class&);
	public:
		Class();

		std::string GetName()const override { return TypeDescriptor<typename Remove_const_if<T>::Type>::Descriptor(); }
	};

	template<typename T>
	Class<T>& GetClass()
	{
		static bool isFirst = true;
		static std::unique_ptr<Class<T>> sharedClass;

		if (isFirst)
		{
			isFirst = false;
			sharedClass = std::make_unique<Class<T>>();
			DescribleClass(*sharedClass);
		}
		return *sharedClass;
	}

	template<typename T>
	Class<T>::Class()
	{
		Reflection::GetClass<T>();
	}
}

