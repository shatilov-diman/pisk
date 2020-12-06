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

#include <pisk/utils/noncopyable.h>

#include <pisk/infrastructure/DataStream.h>

#include <pisk/tools/ComponentsLoader.h>

#include <memory>

#include "Resource.h"

namespace pisk
{
namespace system
{
	class ResourceLoader :
		public core::Component
	{
	public:
		virtual bool can_load(const infrastructure::DataStreamPtr& stream) const noexcept threadsafe = 0;

		virtual ResourcePtr load(infrastructure::DataStreamPtr&& stream) const threadsafe = 0;
	};
	using ResourceLoaderPtr = tools::InterfacePtr<ResourceLoader>;
}
}
