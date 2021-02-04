#pragma once
#include <string>
#include <vector>
#include <array>
#include "Value.h"

namespace Reflection
{
	template<typename T, size_t N, typename...P, size_t...I>
	void TupleToArray(std::array<T, N>& a, std::tuple<P...>& t, std::index_sequence<I...>)
	{
		a = { static_cast<T>(std::get<I>(t))... };
	}

	template<typename T, typename...P>
	auto ToArray(std::tuple<P...>& t)->std::array<T, sizeof...(P)>
	{
		std::array<T, sizeof...(P)> result;
		TupleToArray(result, t, std::make_index_sequence<sizeof...(P)>());
		return result;
	}

	class Method
	{
		std::string mName;
	public:
		Method(const std::string name) : mName(name) {}
		virtual ~Method() = default;

		std::string GetName()const { return mName; }

		template<typename...P> 
		std::shared_ptr<Value> Invoke(nullptr_t, P&&...);
		template<typename C, typename...P> 
		std::shared_ptr<Value> Invoke(C* instance, P&&...);

	private:
		virtual std::shared_ptr<Value> Invoke(nullptr_t, Value*[]) = 0;
		virtual std::shared_ptr<Value> Invoke(void*, Value*[]) = 0;
	};

	template<typename...P>
	std::shared_ptr<Value> Method::Invoke(nullptr_t, P&&...args)
	{
		std::tuple<Value> values = std::make_tuple(&Value(args)...);
		auto arrayValues = ToArray<Value*, P...>(values);
		return Invoke(nullptr, arrayValues.data());
	}

	template<typename C,typename...P>
	std::shared_ptr<Value> Method::Invoke(C* instance, P&&...args)
	{
		std::tuple<Value> values = std::make_tuple(&Value(args)...);
		auto arrayValues = ToArray<Value*, P...>(values);
		return Invoke(instance, arrayValues.data());
	}

	template<typename>
	class DeducedMethod;

	template<typename R, typename...P>
	class DeducedMethod<R(*)(P...)> : public Method
	{
		R(*mMethod)(P...);
	private:
		std::shared_ptr<Value> Invoke(nullptr_t, Value*[])override;
		std::shared_ptr<Value> Invoke(void* instance, Value*[])override;

		template<size_t...I>
		std::shared_ptr<Value> InvokeByIndexSquence(nullptr_t, Value*[], std::index_sequence<I...>);
	public:
		DeducedMethod(const std::string& name, R*(method)(P...))
			:Method(name), mMethod(method) {}

		R Invoke(nullptr_t, P&&...args)
		{
			return (*mMethod)(std::forward<P>(args)...);
		}
	};

	template<typename R, typename... P>
	std::shared_ptr<Value> DeducedMethod<R(*) (P...)>::Invoke(nullptr_t, Value* values[])
	{
		return InvokeByIndexSquence(nullptr, values, std::make_index_sequence<sizeof...(P)>());
	}

	template<typename R,typename...P> 
	std::shared_ptr<Value> DeducedMethod<R(*)(P...)>::Invoke(void* instance, Value* values[])
	{
		return std::make_shared<Value>(nullptr);
	}

	template<typename R, typename ...P>
	template<size_t ...I>
	inline std::shared_ptr<Value> DeducedMethod<R(*)(P...)>::InvokeByIndexSquence(nullptr_t, Value * values[], std::index_sequence<I...>)
	{
		std::shared_ptr<Value> result(new Value);
		result.get()->Set((*mMethod)(std::forward<P>(values[I]->Get<P>()...));
		return result;
	}

	template<typename R, typename C, typename...P>
	class DeducedMethod<R(C::*)(P...)> : public Method
	{
		R(C::*mMethod)(P...);
	private:
		std::shared_ptr<Value> Invoke(nullptr_t, Value*[])override;
		std::shared_ptr<Value> Invoke(void* instance, Value*[])override;

		template<size_t...I>
		std::shared_ptr<Value> InvokeByIndexSquence(void* instance, Value*[], std::index_sequence<I...>);
	public:
		DeducedMethod(const std::string& name, R(C::*method)(P...))
			:Method(name), mMethod(method) {}

		R Invoke(C* instance, P&&...args)
		{
			return (instance->*mMethod)(std::forward<P>(args)...);
		}
	};

	template<typename R, typename C, typename ...P>
	std::shared_ptr<Value> DeducedMethod<R(C::*)(P...)>::Invoke(nullptr_t, Value*[])
	{
		return std::make_shared(nullptr);
	}

	template<typename R, typename C, typename ...P>
	std::shared_ptr<Value> DeducedMethod<R(C::*)(P...)>::Invoke(void* instance, Value*[])
	{

	}

	template<typename R, typename C, typename ...P>
	template<size_t ...I>
	inline std::shared_ptr<Value> DeducedMethod<R(C::*)(P...)>::InvokeByIndexSquence(void * instance, Value *[], std::index_sequence<I...>)
	{
		return std::shared_ptr<Value>();
	}
}
