// Project pisk
// Copyright (C) 2016-2017 Dmitry Shatilov
//
// Original sources:
//   https://github.com/shatilov-diman/pisk/
//   https://bitbucket.org/charivariltd/pisk/
//
// Author contacts:
//   Dmitry Shatilov (e-mail: shatilov.diman@gmail.com; site: https://www.linkedin.com/in/shatilov)
//
//



#pragma once

#include "../defines.h"

#include <functional>
#include <memory>

namespace pisk
{
namespace tools
{
	class Releasable
	{
	protected:
		virtual ~Releasable() {}
	public:
		virtual void release() = 0;
	};

	template <typename Type>
	using unique_relesable_ptr = std::unique_ptr<Type, std::function<void (Releasable*)>>;

	template<typename Type>
	unique_relesable_ptr<Type> make_unique_releasable_raw(Type* raw)
	{
		return unique_relesable_ptr<Type>(raw, [](Releasable* ref) {ref->release(); });
	}
	template<typename Type, typename ... TArgs>
	unique_relesable_ptr<Type> make_unique_releasable(TArgs&& ... args)
	{
		return make_unique_releasable_raw<Type>(new Type(std::forward<TArgs>(args)...));
	}

	template <typename Type>
	using enable_shared_from_this = std::enable_shared_from_this<Type>;

	template <typename Type>
	using weak_relesable_ptr = std::weak_ptr<Type>;

	template <typename Type>
	using shared_relesable_ptr = std::shared_ptr<Type>;

	template<typename Type>
	shared_relesable_ptr<Type> make_shared_releasable_raw(Type* raw)
	{
		return std::shared_ptr<Type>(raw, [](Releasable* ref) {ref->release(); });
	}
	template<typename Type, typename ... TArgs>
	shared_relesable_ptr<Type> make_shared_releasable(TArgs&& ... args)
	{
		return make_shared_releasable_raw<Type>(new Type(std::forward<TArgs>(args)...));
	}
}
}

