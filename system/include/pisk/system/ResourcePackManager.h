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

#include "ResourcePack.h"

namespace pisk
{
namespace system
{
	class ResourceNotFound : public infrastructure::Exception
	{};

	class ResourcePackManager :
		public utils::noncopyable
	{
	public:
		virtual void set_pack(const std::string& pack_name, std::unique_ptr<ResourcePack>&& pack) noexcept = 0;

		virtual void remove_pack(const std::string& pack_name) noexcept = 0;

		virtual void clear() noexcept = 0;
	};
}
}
