#pragma once
#include "Type.h"

namespace Reflection
{
	template<typename T>
	void DescribleType(Type<T>&);

	template<typename T>
	Type<T>& GetType()
	{
		static bool _isFirst = true;
		static std::unique_ptr<Type<T>> _sharedType;

		if (_isFirst)
		{
			_isFirst = false;
			_sharedType = std::make_unique<Type<T>>();
			DescribleType(*_sharedType.get());
		}

		return *_sharedType.get();
	}

#define ReflectionClass(Type)\
	template<> class Reflection::TypeDescriptor<Type>\
	{\
	public:\
		static std::string Descriptor(){return #Type;}\
	};

}