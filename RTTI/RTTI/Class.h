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
}

