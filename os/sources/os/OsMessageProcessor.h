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

#include <memory>
#include <vector>

namespace pisk
{
namespace os
{

class OsMessageProcessor
{
public:
	virtual ~OsMessageProcessor() = default;

	virtual void init() {}

	virtual void loop_once() = 0;

	virtual void deinit() {}

	virtual bool is_stop_requested() const
	{
		return false;
	}

	virtual bool is_ready_to_stop() const
	{
		return true;
	}
};
using OsMessageProcessorPtr = std::unique_ptr<OsMessageProcessor>;

}//namespace os
}//namespace pisk

