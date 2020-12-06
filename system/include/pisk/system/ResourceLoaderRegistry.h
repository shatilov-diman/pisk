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

#include <memory>

#include "ResourceLoader.h"

namespace pisk
{
namespace system
{
	class ResourceFormatNotSupported : public infrastructure::Exception
	{};

	class ResourceLoaderRegistry :
		public core::Component
	{
	public:
		virtual void register_resource_loader(const std::string& resource_type, ResourceLoaderPtr loader) = 0;
	};
}
}
