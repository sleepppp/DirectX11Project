#pragma once
#include <string>
#include <vector>
#include <array>
#include <functional>
#include <stdarg.h>
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
		Method(const std::string& name) : mName(name) {}
		virtual ~Method() = default;

		std::string GetName()const { return mName; }

		template<typename...P>
		std::shared_ptr<Value> Invoke(nullptr_t, P...);
		template<typename C, typename...P>
		std::shared_ptr<Value> Invoke(C* instance, P...);
	private:
		template<typename C>
		std::shared_ptr<Value> InvokeNoneParam(C* instance);
		template<typename C, typename...P>
		std::shared_ptr<Value> InvokeParam(C* instance, P...);
	private:
		virtual std::shared_ptr<Value> InvokeReal(nullptr_t, std::shared_ptr<Value>[]) = 0;
		virtual std::shared_ptr<Value> InvokeReal(void*, std::shared_ptr<Value>[]) = 0;
	};

	template<typename...P>
	std::shared_ptr<Value> Method::Invoke(nullptr_t, P...args)
	{
		if (sizeof...(P) == 0)
		{
			return InvokeNoneParam(nullptr);
		}
		else
		{
			return InvokeParam(nullptr, args...);
		}

	}

	template<typename C, typename...P>
	std::shared_ptr<Value> Method::Invoke(C* instance, P...args)
	{
		if (sizeof...(P) == 0)
		{
			return InvokeNoneParam<C>(instance);
		}
		else
		{
			return InvokeParam(instance, args...);
		}
	}

	template<typename C>
	inline std::shared_ptr<Value> Method::InvokeNoneParam(C * instance)
	{
		std::array<std::shared_ptr<Value>, 0> param;
		return InvokeReal(instance, param.data());
	}

	template<typename C, typename ...P>
	inline std::shared_ptr<Value> Method::InvokeParam(C * instance, P ...args)
	{
		auto values = std::make_tuple(std::shared_ptr<Value>(new Value(args))...);
		auto arrayValues = ToArray<std::shared_ptr<Value>>(values);

		return InvokeReal(instance, arrayValues.data());
	}

	template<typename>
	class DeducedMethod;

	template<typename...P>
	class DeducedMethod<void(*)(P...)> : public Method
	{
		void(*mMethod)(P...);
	private:
		std::shared_ptr<Value> InvokeReal(nullptr_t, std::shared_ptr<Value>[])override;
		std::shared_ptr<Value> InvokeReal(void* instance, std::shared_ptr<Value>[])override;

		template<size_t...I>
		void InvokeByIndexSquence(nullptr_t, std::shared_ptr<Value>[], std::index_sequence<I...>);
	public:
		DeducedMethod(const std::string& name, void(*method)(P...))
			:Method(name), mMethod(method) {}

		void Invoke(nullptr_t, P&&...args)
		{
			(*mMethod)(std::forward<P>(args)...);
		}
	};

	template<typename...P>
	std::shared_ptr<Value> DeducedMethod<void(*)(P...)>::InvokeReal(nullptr_t, std::shared_ptr<Value>values[])
	{
		InvokeByIndexSquence(nullptr, values, std::make_index_sequence<sizeof...(P)>());
		return nullptr;
	}

	template<typename...P>
	std::shared_ptr<Value> DeducedMethod<void(*)(P...)>::InvokeReal(void* instance,std::shared_ptr<Value>[])
	{
		//전역 함수가 들어오므로 이쪽 경로를 탔다면 동작 X
		return nullptr;
	}

	template<typename...P> template<size_t...I>
	void DeducedMethod<void(*)(P...)>::InvokeByIndexSquence(nullptr_t, std::shared_ptr<Value> values[], std::index_sequence<I...>)
	{
		(*mMethod)(values[I].get()->Get<P>()...);
	}

	template<typename R, typename...P>
	class DeducedMethod<R(*)(P...)> : public Method
	{
		R(*mMethod)(P...);
	private:
		std::shared_ptr<Value> InvokeReal(nullptr_t, std::shared_ptr<Value>[])override;
		std::shared_ptr<Value> InvokeReal(void* instance, std::shared_ptr<Value>[])override;

		template<size_t...I>
		std::shared_ptr<Value> InvokeByIndexSquence(nullptr_t, std::shared_ptr<Value>[], std::index_sequence<I...>);
	public:
		DeducedMethod(const std::string& name, R(*method)(P...))
			:Method(name), mMethod(method) {}

		R Invoke(nullptr_t, P&&...args)
		{
			return (*mMethod)(std::forward<P>(args)...);
		}
	};

	template<typename R, typename... P>
	std::shared_ptr<Value> DeducedMethod<R(*) (P...)>::InvokeReal(nullptr_t, std::shared_ptr<Value> values[])
	{
		return InvokeByIndexSquence(nullptr, values, std::make_index_sequence<sizeof...(P)>());
	}

	template<typename R,typename...P> 
	std::shared_ptr<Value> DeducedMethod<R(*)(P...)>::InvokeReal(void* instance, std::shared_ptr<Value> values[])
	{
		return nullptr;
	}

	template<typename R, typename ...P>
	template<size_t ...I>
	inline std::shared_ptr<Value> DeducedMethod<R(*)(P...)>::InvokeByIndexSquence(nullptr_t, std::shared_ptr<Value> values[], std::index_sequence<I...>)
	{
		std::shared_ptr<Value> result(new Value);
		result.get()->Set<R>((*mMethod)(values[I].get()->Get<P>()...));
		return result;
	}

	template<typename C, typename...P>
	class DeducedMethod<void(C::*)(P...)> : public Method
	{
		void(C::*mMethod)(P...);
	private:
		std::shared_ptr<Value> InvokeReal(nullptr_t, std::shared_ptr<Value>[])override;
		std::shared_ptr<Value> InvokeReal(void* instance, std::shared_ptr<Value>[])override;

		template<size_t...I>
		void InvokeByIndexSquence(void* instance, std::shared_ptr<Value>[], std::index_sequence<I...>);
	public:
		DeducedMethod(const std::string& name, void(C::*method)(P...))
			:Method(name), mMethod(method) {}

		void Invoke(C* instance, P&&...args)
		{
			(instance->*mMethod)(std::forward<P>(args)...);
		}
	};

	template<typename C, typename...P>
	std::shared_ptr<Value> DeducedMethod<void(C::*)(P...)>::InvokeReal(nullptr_t, std::shared_ptr<Value>[])
	{
		return nullptr;
	}
	template<typename C, typename...P>
	std::shared_ptr<Value> DeducedMethod<void(C::*)(P...)>::InvokeReal(void* instance, std::shared_ptr<Value> values[])
	{
		InvokeByIndexSquence(instance, values, std::make_index_sequence<sizeof...(P)>());
		return nullptr;
	}

	template<typename C, typename...P> template<size_t...I>
	void DeducedMethod<void(C::*)(P...)>::InvokeByIndexSquence(void* instance, std::shared_ptr<Value> values[], std::index_sequence<I...>)
	{
		C* classInstance = static_cast<C*>(instance);
		(classInstance->*mMethod)(values[I].get()->Get<P>()...);
	}

	template<typename R, typename C, typename...P>
	class DeducedMethod<R(C::*)(P...)> : public Method
	{
		R(C::*mMethod)(P...);
	private:
		std::shared_ptr<Value> InvokeReal(nullptr_t, std::shared_ptr<Value>[])override;
		std::shared_ptr<Value> InvokeReal(void* instance, std::shared_ptr<Value>[])override;

		template<size_t...I>
		std::shared_ptr<Value> InvokeByIndexSquence(void* instance, std::shared_ptr<Value>[], std::index_sequence<I...>);
	public:
		DeducedMethod(const std::string& name, R(C::*method)(P...))
			:Method(name), mMethod(method) {}

		R Invoke(C* instance, P&&...args)
		{
			return (instance->*mMethod)(std::forward<P>(args)...);
		}
	};

	template<typename R, typename C, typename ...P>
	std::shared_ptr<Value> DeducedMethod<R(C::*)(P...)>::InvokeReal(nullptr_t, std::shared_ptr<Value>[])
	{
		return nullptr;
	}

	template<typename R, typename C, typename ...P>
	std::shared_ptr<Value> DeducedMethod<R(C::*)(P...)>::InvokeReal(void* instance, std::shared_ptr<Value> values[])
	{
		return InvokeByIndexSquence(instance, values, std::make_index_sequence<sizeof...(P)>());
	}

	template<typename R, typename C, typename ...P> template<size_t...I>
	inline std::shared_ptr<Value> DeducedMethod<R(C::*)(P...)>::InvokeByIndexSquence(void * instance, std::shared_ptr<Value>values[], std::index_sequence<I...>)
	{
		std::shared_ptr<Value> result(new Value());
		C* classInstance = static_cast<C*>(instance);
		result.get()->Set<R>((classInstance->*mMethod)(values[I].get()->Get<P>()...));
		return result;
	}

}
