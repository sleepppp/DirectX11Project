#pragma once
#include "Field.h"
#include "Method.h"
#include <functional>
#include <map>
#include <memory>
#include <set>

namespace Reflection
{
	template<typename> class Class;

	class ClassSharedData
	{
		template<typename> friend class ClassBase;

		static int64_t _nextTypeid;

		int64_t mTypeid;
		std::map<std::string, std::shared_ptr<Method>> mMethods;
		std::map<std::string, std::shared_ptr<Field>> mFields;
		std::set<void*> mInstance;
	public:
		ClassSharedData();
	};

	class ClassUnknown
	{
		static std::map<void*, ClassUnknown*>& Instances();
	protected:
		virtual int64_t GetTypeid()const = 0;
		virtual std::shared_ptr<Method> GetDeclaredMethodFromDescriptin(const std::string&) = 0;
		virtual std::shared_ptr<Field> GetDeclaredFieldFromDescriptin(const std::string&) = 0;

		void TrackInstance(void*);
		void ForgetInstance(void*);

		static ClassSharedData& ClassSharedData(const std::string&);
	public:
		static ClassUnknown* GetClass(void*);
		static ClassUnknown* GetClass(const std::shared_ptr<void>& ptr)
		{
			return GetClass(ptr.get());
		}
		virtual std::string GetName()const = 0;

		std::shared_ptr<Method> GetDeclaredMethod(const std::string&)const;
		template<typename...P> std::shared_ptr<Method> GetDeclaredMethod(const std::string&, const Class<P>&...)const;
		virtual std::vector<std::shared_ptr<Method>> GetDeclaredMethods()const = 0;

		std::shared_ptr<Field> GetDeclaredField(const std::string&)const;
		virtual std::vector<std::shared_ptr<Field>> GetDeclaredFields() const = 0;

		bool IsAssignableFrom(ClassUnknown&)const;
		virtual bool IsInstance(void*)const;
		bool IsInstance(const std::shared_ptr<void>& ptr)const { return IsInstance(ptr.get()); }
	};

	enum LeakPtrTag{LeakPtr};

	template<typename T> 
	class ClassBase : public ClassUnknown
	{
		static ClassSharedData* _sharedData;
	protected:
		template<typename M> void SetDeclaredMethod(const std::string&, M);
		template<typename F> void SetDeclaredField(const std::string&, F);

		int64_t GetTypeid()const override { return _sharedData->mTypeid; }
		std::shared_ptr<Method> GetDeclaredMethodFromDescription(const std::string&)const override;
		std::shared_ptr<Field> GetDeclaredFieldFromDescription(const std::string&)const override;

		void DeleteInstance(T*);

		ClassBsae()
		{
			if (!_sharedData)
			{
				_sharedData = &ClassUnknown::ClassSharedData(TypeDescription<T>::Descriptor());
			}
		}
	public:
		std::vector<std::shared_ptr<Method>> GetDeclaredMethods()const override;
		std::vector<std::shared_ptr<Field>> GetDeclaredFields()const override;

		template<typename...Args> T* NewInstance(LeakPtrTag, Args&&...args);
		template<typename... Args> std::shared_ptr<T> NewInstance(Args&&...args);
		bool IsInstance(void*)const override;
		bool IsInstance(const std::shared_ptr<void>& ptr) { return IsInstance(ptr.get()); }
	};

	template<typename T, bool IsPointer, bool IsReference> struct RemoveConstIfHelper { typedef T type; };
	template<typename T> struct RemoveConstIfHelper<T, false, false> { typedef typename std::remove_cv<T>::type type; };
	template<typename T> struct RemoveConstIf : public RemoveConstIfHelper < T, std::is_pointer<T>::value, std::is_reference<T>::value> {};

	template<typename T> 
	class Class : public ClassBase<typename TypeValueTraits<T>::basetype>
	{
		friend T;
		friend void DescribeClass<T>(Class&);
	public:
		Class();

		std::string GetName()const override { return TypeDescriptor<typename RemoveConstIf<T>::type>::Descriptor(); }
	};

	inline ClassUnknown* ClassUnknown::GetClass(void* instance)
	{
		if (!instance || Instances().count(instance) == 0)
		{
			return nullptr;
		}
		return Instances()[instance];
	}

	inline std::shared_ptr<Method> ClassUnknown::GetDeclaredMethod(const std::string& methodName)const
	{
		std::string descriptor = MethodDescriptor(GetName(), methodName, 0, nullptr);
		return GetDeclaredMethodFromDescriptin(descriptor);
	}

	template<typename... P> 
	std::shared_ptr<Method> ClassUnknown::GetDeclaredMethod(const std::string& methodName, const Class<P>&...parametorTypes)const
	{
		std::string parameterTypeDescriptors[] = { parameterTypes.GetName()... };
		std::string descriptor = MethodDescriptor(GetName(), methodName, sizeof(P), parameterTypeDescriptors);
		return GetDeclaredMethodFromDescriptin(descriptor);
	}

	inline std::shared_ptr<Field> ClassUnknown::GetDeclaredField(const std::string& fieldName)const
	{
		std::string descriptor = FieldDescriptor(GetName(), fieldName);
		return GetDeclaredFieldFromDescription(descriptor);
	}

	inline bool ClassUnknown::IsAssignableFrom(ClassUnknown& other)
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
		Instances().erase(instance);
	}

	template<typename T> ClassSharedData* ClassBase<T>::_sharedData;

	template<typename T>
	std::vector<std::shared_ptr<Method>> ClassBase<T>::GetDeclaredMethods()const
	{
		std::vector<std::shared_ptr<Field>> fields;
		for (auto& field : _sharedData->mFields)
		{
			fields.push_back(field.second);
		}
		return fields;
	}

	template<typename T> template<typename...Args>
	T* ClassBase<T>::NewInstance(LeakPtrTag, Args&&...args)
	{
		T* instance = new T(args);
		ClassUnknown::TrackInstance(instance);
		_sharedData->mInstances.insert(instance);
		return instance;
	}

	template<typename T>template<typename...Args>
	std::shared_ptr<T> ClassBase<T>::NewInstance(Args&&... args)
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

		return std::shared_ptr<T>(instance, Deleter({ [=](T* ptr) {DeleteInstance(ptr); } }))
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
		return _sharedData->mInstance.count(ptr) > 0;
	}

	template<typename T> template<typename M>
	void ClassBase<T>::SetDeclaredMethod(const std::string& methodName, M method)
	{
		std::string descriptor = MethodDescriptor<M>::Descriptor(GetName(), methodName);
		_sharedData->mMethods[descriptor] = std::make_shared<DeducedMethod<M>>(methodName, method);
	}
}

