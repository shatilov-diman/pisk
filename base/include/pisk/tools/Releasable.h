// Project pisk
// Copyright (C) 2016-2017 Dmitry Shatilov
//
// This file is a part of the module base of the project pisk.
// This file is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
// Additional restriction according to GPLv3 pt 7:
// b) required preservation author attributions;
// c) required preservation links to original sources
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

	template<typename Type>
	std::shared_ptr<Type> make_shared_releasable_raw(Type* raw)
	{
		return std::shared_ptr<Type>(raw, [](Releasable* ref) {ref->release(); });
	}
	template<typename Type, typename ... TArgs>
	std::shared_ptr<Type> make_shared_releasable(TArgs&& ... args)
	{
		return make_shared_releasable_raw<Type>(new Type(std::forward<TArgs>(args)...));
	}
}
}

