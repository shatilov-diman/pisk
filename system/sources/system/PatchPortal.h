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

#include <pisk/system/PatchPtr.h>

#include <memory>

namespace pisk
{
namespace system
{
	class PatchGate
	{
	public:
		virtual ~PatchGate() {}

		virtual PatchPtr pop() threadsafe = 0;

		virtual void push(const PatchPtr& patch) threadsafe = 0;
	};
	using PatchGatePtr = std::unique_ptr<PatchGate>;

	class PatchPortal
	{
	public:
		virtual ~PatchPortal() {}

		virtual PatchGatePtr make_gate() threadsafe = 0;
	};
	using PatchPortalPtr = std::unique_ptr<PatchPortal>;
}
}

