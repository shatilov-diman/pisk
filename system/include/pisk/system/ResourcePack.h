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

namespace pisk
{
namespace system
{
	class ResourcePack :
		public utils::noncopyable
	{
	public:
		virtual ~ResourcePack() = default;

		virtual infrastructure::DataStreamPtr open(const std::string& rid) const noexcept threadsafe = 0;
	};
	using ResourcePackPtr = std::unique_ptr<ResourcePack>;
}
}
