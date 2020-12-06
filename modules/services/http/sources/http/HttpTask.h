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

#include <pisk/http/structs.h>

#include <future>

namespace pisk
{
namespace services
{
namespace http
{
	struct HttpTask
	{
		Request request;
		Response response;
		std::promise<Response> response_promise;
	};
	using HttpTaskPtr = std::unique_ptr<HttpTask>;
}
}
}

