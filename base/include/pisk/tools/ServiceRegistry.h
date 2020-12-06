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
	class ServiceRegistry
	{
	public:
		template<typename Interface>
		using InterfacePtr = InterfacePtr<Interface>;

		using UID = utils::keystring;

		template<typename Interface>
		InterfacePtr<Interface> get() const
		{
			return get<Interface>(Interface::uid);
		}
		template<typename Interface>
		InterfacePtr<Interface> get(const UID& uid) const
		{
			SafeComponentPtr instance = find(uid);
			if (instance == nullptr)
				return {};
			return instance.cast<Interface>();
		}

	private:
		virtual SafeComponentPtr find(const UID& uid) const = 0;
	};
}
}

