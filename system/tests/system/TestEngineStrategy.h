// Project pisk
// Copyright (C) 2016-2017 Dmitry Shatilov
//
// This file is a part of the module system of the project pisk.
// This file is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
// Additional restriction according to GPLv3 pt 7:
// b) required preservation author attributions;
// c) required preservation links to original sources
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

#include <pisk/utils/property_tree.h>
#include <pisk/infrastructure/Exception.h>

#include <pisk/system/PatchPtr.h>
#include <pisk/system/EngineStrategy.h>

#include "../../sources/system/Engine.h"
#include "../../sources/system/EngineTask.h"
#include "../../sources/system/EngineComponentFactory.h"

#include <atomic>
#include <thread>

class EngineStrateyTestBase :
	public pisk::system::EngineStrategy
{
public:
	static std::atomic<std::thread::id> thread_id;
	static std::atomic_bool destroied;

	EngineStrateyTestBase()
	{
		thread_id = std::thread::id();
		destroied = false;
	}
	virtual ~EngineStrateyTestBase() override
	{
		destroied = true;
	}

	virtual Configure on_init_app() override
	{
		thread_id = std::this_thread::get_id();
		assert(thread_id != std::thread::id());
		return {};
	}

	virtual void on_deinit_app() override
	{
		assert(thread_id == std::this_thread::get_id());
	}

	virtual void patch_scene(const pisk::system::PatchPtr& scene) override
	{
		assert(thread_id == std::this_thread::get_id());
		if (scene == nullptr)
			throw pisk::infrastructure::NullPointerException();
	}

	virtual void update() override
	{}
};

class TestPatchGate:
	public pisk::system::PatchGate
{
public:
	virtual pisk::system::PatchPtr pop() threadsafe
	{
		return {};
	}

	virtual void push(const pisk::system::PatchPtr&) threadsafe
	{}
};

template <typename Strategy>
std::unique_ptr<pisk::system::impl::EngineTask> make_engine_task(pisk::system::PatchGatePtr&& gate)
{
	if (gate == nullptr)
		throw pisk::infrastructure::NullPointerException();
	return std::make_unique<pisk::system::impl::EngineTask>(
		std::move(gate),
		[](pisk::system::PatchRecipient&) {
			return std::make_unique<Strategy>();
		}
	);
}

