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

#include "../utils/noncopyable.h"
#include "../tools/Releasable.h"

#include <typeinfo>
#include <memory>

namespace pisk
{
namespace core
{
	class Component :
		public tools::Releasable,
		public utils::noncopyable
	{
	};
}
}
