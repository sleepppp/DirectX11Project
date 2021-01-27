#pragma once
#include "Value.h"
#include <array>
#include <vector>

namespace Reflection
{
	class ClassUnknown;

	class Method
	{
		std::string mName;
	public:
		Method(const std::string& name)
			:mName(name) {}
		virtual ~Method() = default;

		std::string GetName()const { return mName; }
		virtual std::vector<ClassUnknown*> GetParameterTypes() = 0;

		template<typename ...P>
		std::shared_ptr<Value> Invoke(nullptr_t, P&&...);
		template<typename C,typename...P> 
		std::shared_ptr<Value> Invoke(C& receiver, P&&...);

		virtual std::shared_ptr<Value> Invoke(nullptr_t, Value*[]) = 0;
		virtual std::shared_ptr<Value> Invoke(void* receiver, Value*[]) = 0;
	};

	template<typename> class DeducedMethod;

	template<typename R,typename...P>
	class DeducedMethod<R(*)(P...)> : public Method
	{
		R(*mMethod)(P...);

		std::vector<ClassUnknown*> GetParameterTypes()override;
		std::shared_ptr<Value> Invoke(nullptr_t, Value*[])override;
		std::shared_ptr<Value> Invoke(void* receiver, Value*[])override;

		template<size_t...I>
		std::shared_ptr<Value> Forward(nullptr_t, Value*[], std::index_sequence<I...>);

	public:
		DeducedMethod(const std::string& name, R(*method)(P...))
			:Method(name), mMethod(method) {}

		R Invoke(nullptr_t, P&&... args)
		{
			return (*mMethod)(std::forward<P>(args));
		}
	};

	template<typename R,typename C, typename...P>
	class DeducedMethod<R(C::*)(P...)> : public Method
	{
		R(C::*mMethod)(P...);

		std::vector<ClassUnknown*> GetParameterTypes() override;
		std::shared_ptr<Value> Invoke(nullptr_t, Value*[]) override;
		std::shared_ptr<Value> Invoke(void* receiver, Value*[]) override;

		template<size_t... I> std::shared_ptr<Value> Forward(void* receiver, Value*[], std::index_sequence<I...>);
	public:
		DeducedMethod(const std::string& name, R(C::*method)(P...))
			:Method(name), mMethod(method) {}

		R Invoke(C& receiver, P&&...args)
		{
			return (receiver.*mMethod)(std::forward<P>(args)...);
		}
	};

	template<typename T, size_t N, typename... P, size_t... I>
	void Tuple_to_array(std::array<T, N>& a, std::tuple<P...>& t, std::index_sequence<I...>)
	{
		a = { static_cast<T>(std::get<I>(t))... };
	}

	template<typename T, typename... P>
	auto To_array(std::tuple<P...>& t)->std::array<T, sizeof...(P)>
	{
		std::array<T, sizeof...(P)> result;
		tuple_to_array(result, t, std::make_index_sequence<sizeof...(P)>());
		return result;
	}

	template<typename... P>
		std::shared_ptr<Value> Method::Invoke(nullptr_t, P&&... arguments)
	{
		std::tuple<TypedValue<P>...> typedValues = std::make_tuple(TypeValue<P>(std::forward<P>(arguments))...);
		auto values = To_array<Value*>(typedValues);
		return invoke(nullptr, values.data());
	}

	template<typename C, typename... P>
	std::shared_ptr<Value> Method::Invoke(C& receiver, P&&... arguments)
	{
		std::tuple<TypedValue<P>...> typedValues = std::make_tuple(TypeValue<P>(std::forward<P>(arguments))...);
		auto values = To_array<Value*>(typedValues);
		return invoke(&receiver, values.data());
	}

	template<typename R, typename... P>
	std::vector<ClassUnknown*> DeducedMethod<R(*) (P...)>::GetParameterTypes()
	{
		if (sizeof...(P) == 0)
			return std::vector<ClassUnknown*>();

		auto typeClasses = std::make_tuple(&reflect::getClass<P>()...);
		auto classes = To_array<ClassUnknown*>(typeClasses);
		return std::vector<ClassUnknown*>(sizeof...(P), classes.front());
	}

	template<typename R, typename... P>
	std::shared_ptr<Value> DeducedMethod<R(*) (P...)>::Invoke(nullptr_t, Value* values[])
	{
		return forward(nullptr, values, std::make_index_sequence<sizeof...(P)>());
	}

	template<typename R, typename... P>
	std::shared_ptr<Value> DeducedMethod<R(*) (P...)>::Invoke(void* receiver, Value* values[])
	{
		return nullptr;
	}

	template<typename R, typename... P> template<size_t... I>
	std::shared_ptr<Value> DeducedMethod<R(*) (P...)>::Forward(nullptr_t, Value* values[], std::index_sequence<I...>)
	{
		return reflect::forward<R>(0, mMethod, ValueCastFunction<P>::Cast(*values[I])...);
	}

	template<typename R, typename M, typename... P>
	std::shared_ptr<Value> forward(typename std::enable_if<std::is_void<R>::value>::type*, M method, P&&... arguments)
	{
		(*method)(std::forward<P>(arguments)...);
		return std::make_shared<Void>();
	}

	template<typename R, typename M, typename... P>
	std::shared_ptr<Value> forward(typename std::enable_if<!std::is_void<R>::value>::type*, M method, P&&... arguments)
	{
		return std::make_shared<TypeValue<R>>((*method)(std::forward<P>(arguments)...));
	}

	template<typename R, typename C, typename... P>
	std::vector<ClassUnknown*> DeducedMethod<R(C::*) (P...)>::GetParameterTypes()
	{
		if (sizeof...(P) == 0)
			return std::vector<ClassUnknown*>();

		auto typeClasses = std::make_tuple(&reflect::getClass<P>()...);
		auto classes = to_array<ClassUnknown*>(typeClasses);
		return std::vector<ClassUnknown*>(sizeof...(P), classes.front());
	}

	template<typename R, typename C, typename... P>
	std::shared_ptr<Value> DeducedMethod<R(C::*) (P...)>::Invoke(nullptr_t, Value* values[])
	{
		return nullptr;
	}

	template<typename R, typename C, typename... P>
	std::shared_ptr<Value> DeducedMethod<R(C::*) (P...)>::Invoke(void* receiver, Value* values[])
	{
		return forward(receiver, values, std::make_index_sequence<sizeof...(P)>());
	}

	template<typename R, typename C, typename... P> template<size_t... I>
	std::shared_ptr<Value> DeducedMethod<R(C::*) (P...)>::Forward(void* receiver, Value* values[], std::index_sequence<I...>)
	{
		return reflect::forward<R>(0, mMethod, *(static_cast<C*>(receiver)), ValueCastFunction<P>::Cast(*values[I])...);
	}

	template<typename R, typename M, typename C, typename... P>
	std::shared_ptr<Value> forward(typename std::enable_if<std::is_void<R>::value>::type*, M method, C& receiver, P&&... arguments)
	{
		(receiver.*method)(std::forward<P>(arguments)...);
		return std::make_shared<Void>();
	}

	template<typename R, typename M, typename C, typename... P>
	std::shared_ptr<Value> forward(typename std::enable_if<!std::is_void<R>::value>::type*, M method, C& receiver, P&&... arguments)
	{
		return std::make_shared<TypeValue<R>>((receiver.*method)(std::forward<P>(arguments)...));
	}
}

