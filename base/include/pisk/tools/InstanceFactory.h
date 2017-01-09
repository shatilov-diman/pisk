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
#include "../utils/noncopyable.h"
#include "../utils/keystring.h"
#include "../infrastructure/Exception.h"

#include "Releasable.h"
#include "ComponentPtr.h"

#include <type_traits>
#include <map>

namespace pisk
{
namespace tools
{
	class InstanceFactory
	{
		virtual SafeComponentPtr safe_instance(const std::shared_ptr<core::Component>& instance) const = 0;
	public:

		template <typename Instance, typename ... Args>
		InterfacePtr<Instance> make(Args ... args) const
		{
			auto instance = tools::make_shared_releasable<Instance>(std::forward<Args>(args)...);
			return safe_instance(instance).template cast<Instance>();
		}
	};

}
}
