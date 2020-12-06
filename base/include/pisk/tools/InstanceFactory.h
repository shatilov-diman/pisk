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

