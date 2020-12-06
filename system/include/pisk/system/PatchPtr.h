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

#include <pisk/utils/safequeue.h>
#include <pisk/utils/property_tree.h>

namespace pisk
{
namespace system
{
	using Patch = utils::property;
	using PatchPtr = std::shared_ptr<const Patch>;
	using PatchQueue = utils::safequeue<PatchPtr>;
	using PatchQueuePtr = std::shared_ptr<PatchQueue>;
}
}

