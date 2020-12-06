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

#include <pisk/defines.h>
#include <pisk/utils/property_tree.h>
#include <pisk/infrastructure/Exception.h>

#include "ResourceManager.h"

namespace pisk
{
namespace system
{
	class PropertyTreeResource :
		public Resource
	{
	public:
		constexpr static const char* resource_type = "property_tree";

		virtual utils::property get() threadsafe const noexcept = 0;
	};

}
}

