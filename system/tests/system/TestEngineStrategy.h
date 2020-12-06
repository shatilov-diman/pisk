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

	virtual void prepatch() override
	{}

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
std::unique_ptr<pisk::system::impl::EngineTask> make_engine_task(pisk::system::PatchGatePtr&& gate, pisk::system::EngineSynchronizerSlavePtr&& sync)
{
	if (gate == nullptr)
		throw pisk::infrastructure::NullPointerException();
	return std::make_unique<pisk::system::impl::EngineTask>(
		[](pisk::system::PatchRecipient&) {
			return std::make_unique<Strategy>();
		},
		std::move(sync),
		std::move(gate)
	);
}

