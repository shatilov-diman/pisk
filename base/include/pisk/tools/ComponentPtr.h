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
#include "../infrastructure/Module.h"
#include "../core/Component.h"

namespace pisk
{
namespace tools
{
	template<typename Interface>
	using InterfacePtr = infrastructure::ModuleHolderProxy<Interface>;

	template<typename Interface>
	using InterfaceWPtr = infrastructure::ModuleHolderProxy<Interface, tools::weak_relesable_ptr<Interface>>;

	using SafeComponentPtr = InterfacePtr<core::Component>;
}
}

