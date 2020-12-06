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

#include "ReflectedItem.h"

namespace pisk
{
namespace model
{
	template <typename cv_property = utils::property>
	using ReflectedPropertyBase = ReflectedItemBase<cv_property>;

	using ConstReflectedProperty = ReflectedPropertyBase<const utils::property>;
	using ReflectedProperty = ReflectedPropertyBase<utils::property>;
}
}

