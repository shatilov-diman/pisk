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

#include <pisk/tools/ComponentPtr.h>

#include "structs.h"
#include "utils.h"

#include <future>

namespace pisk
{
namespace services
{
namespace http
{
	class Service :
		public core::Component
	{
	public:
		constexpr static const char* uid = "http";

		virtual std::future<Response> request(const Request& request) noexcept threadsafe = 0;
	};
	using ServicePtr = tools::InterfacePtr<Service>;
}
}
}

