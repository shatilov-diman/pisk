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


#include <pisk/gtest.h>
#include <pisk/infrastructure/Exception.h>

#include "TestEngineStrategy.h"

using namespace pisk;
using namespace pisk::system;
using namespace pisk::system::impl;

using namespace ::testing;

namespace pisk {
namespace system {
	EngineSynchronizerPtr make_engine_synchronizer();
}//namespace system
}//namespace pisk

class SynchSlaveMock :
	public system::EngineSynchronizerSlave
{
	pisk::system::EngineSynchronizerSlavePtr slave;

	void release_impl()
	{
		slave.reset();
	}

public:
	explicit SynchSlaveMock(pisk::system::EngineSynchronizerSlavePtr _slave):
		slave(std::move(_slave))
	{
		ON_CALL(*this, release())
			.WillByDefault(Invoke(this, &SynchSlaveMock::release_impl));

		ON_CALL(*this, is_stop_requested())
			.WillByDefault(Invoke(slave.get(), &EngineSynchronizerSlave::is_stop_requested));
		ON_CALL(*this, notify_ready())
			.WillByDefault(Invoke(slave.get(), &EngineSynchronizerSlave::notify_ready));
		ON_CALL(*this, wait_initialize_signal())
			.WillByDefault(Invoke(slave.get(), &EngineSynchronizerSlave::wait_initialize_signal));
		ON_CALL(*this, notify_initialize_finished())
			.WillByDefault(Invoke(slave.get(), &EngineSynchronizerSlave::notify_initialize_finished));
		ON_CALL(*this, wait_loop_begin_signal())
			.WillByDefault(Invoke(slave.get(), &EngineSynchronizerSlave::wait_loop_begin_signal));
		ON_CALL(*this, notify_loop_finished())
			.WillByDefault(Invoke(slave.get(), &EngineSynchronizerSlave::notify_loop_finished));
		ON_CALL(*this, wait_deinitialize_signal())
			.WillByDefault(Invoke(slave.get(), &EngineSynchronizerSlave::wait_deinitialize_signal));
		ON_CALL(*this, notify_deinitialize_finished())
			.WillByDefault(Invoke(slave.get(), &EngineSynchronizerSlave::notify_deinitialize_finished));
	}

	MOCK_METHOD0(release, void());

	MOCK_METHOD0(is_stop_requested, bool());
	MOCK_METHOD0(notify_ready, void());

	MOCK_METHOD0(wait_initialize_signal, void());
	MOCK_METHOD0(notify_initialize_finished, void());

	MOCK_METHOD0(wait_loop_begin_signal, void());
	MOCK_METHOD0(notify_loop_finished, void());

	MOCK_METHOD0(wait_deinitialize_signal, void());
	MOCK_METHOD0(notify_deinitialize_finished, void());
};

class EngineStrategyMock :
	public system::EngineStrategy
{
public:
	class Proxy :
		public system::EngineStrategy
	{
		EngineStrategyMock& mock;
	public:
		explicit Proxy(EngineStrategyMock& mock) :
			mock(mock)
		{}
		system::EngineStrategy::Configure on_init_app() {
			return mock.on_init_app();
		}
		void on_deinit_app() {
			return mock.on_deinit_app();
		}
		void prepatch() {
			return mock.prepatch();
		}
		void patch_scene(const system::PatchPtr& patch) {
			return mock.patch_scene(patch);
		}
		void update() {
			return mock.update();
		}
	};

	MOCK_METHOD0(on_init_app, system::EngineStrategy::Configure());
	MOCK_METHOD0(on_deinit_app, void());
	MOCK_METHOD0(prepatch, void());
	MOCK_METHOD1(patch_scene, void(const system::PatchPtr& patch));
	MOCK_METHOD0(update, void());
};

class PatchGateMock :
	public system::PatchGate
{
public:
	class Proxy :
		public system::PatchGate
	{
		PatchGateMock& mock;
	public:
		explicit Proxy(PatchGateMock& mock) :
			mock(mock)
		{}
		system::PatchPtr pop() {
			return mock.pop();
		}
		void push(const system::PatchPtr& patch) {
			return mock.push(patch);
		}
	};

	MOCK_METHOD0(pop, system::PatchPtr());
	MOCK_METHOD1(push, void(const system::PatchPtr& patch));
};

//TODO: refactor it: EXPECT_CALL can not in threadsafe
TEST(engine_task, DISABLED_check_synch)
{
	auto synch = make_engine_synchronizer();
	SynchSlaveMock synch_mock(synch->make_slave());
	EngineStrategyMock strategy_mock;
	PatchGateMock patch_gate_mock;

	auto strategy_factory_mock = [&strategy_mock](system::PatchRecipient&) -> system::EngineStrategyPtr {
		return std::make_unique<EngineStrategyMock::Proxy>(strategy_mock);
	};
	auto synch_mock_ptr = tools::make_unique_releasable_raw<system::EngineSynchronizerSlave>(&synch_mock);
	auto patch_gate_mock_ptr = std::make_unique<PatchGateMock::Proxy>(patch_gate_mock);

	EXPECT_CALL(synch_mock, notify_ready());
	EXPECT_CALL(synch_mock, wait_initialize_signal());
	auto task = std::make_shared<EngineTask>(strategy_factory_mock, std::move(synch_mock_ptr), std::move(patch_gate_mock_ptr));
	synch->wait_all_ready();

	EXPECT_CALL(strategy_mock, on_init_app()).WillOnce(Return(system::EngineStrategy::Configure{}));
	EXPECT_CALL(synch_mock, notify_initialize_finished());
	EXPECT_CALL(synch_mock, wait_loop_begin_signal());
	synch->initialize_signal();
	synch->wait_all_initialized();

	system::PatchPtr patch = std::make_shared<system::Patch>();

	EXPECT_CALL(synch_mock, is_stop_requested()).WillRepeatedly(Return(false));
	EXPECT_CALL(patch_gate_mock, pop()).WillRepeatedly(Return(patch));
	EXPECT_CALL(strategy_mock, prepatch()).Times(AnyNumber());
	EXPECT_CALL(strategy_mock, patch_scene(patch)).Times(AnyNumber());
	EXPECT_CALL(strategy_mock, update()).Times(AnyNumber());
	synch->run_loop_signal();
	std::this_thread::sleep_for(std::chrono::milliseconds(30));

	EXPECT_CALL(synch_mock, is_stop_requested()).Times(AnyNumber());
	EXPECT_CALL(synch_mock, notify_loop_finished());
	EXPECT_CALL(synch_mock, wait_deinitialize_signal());
	synch->stop_all();
	synch->wait_all_loop_finished();

	EXPECT_CALL(strategy_mock, on_deinit_app());
	EXPECT_CALL(synch_mock, notify_deinitialize_finished());
	synch->deinitialize_signal();
	synch->wait_all_deinitialized();

	EXPECT_CALL(synch_mock, release());
	task.reset();
}

TEST(engine_task, exception_when_gate_is_null)
{
	auto synch = make_engine_synchronizer();
	SynchSlaveMock synch_mock(synch->make_slave());
	EngineStrategyMock strategy_mock;
	PatchGateMock patch_gate_mock;

	auto strategy_factory_mock = [&strategy_mock](system::PatchRecipient&) -> system::EngineStrategyPtr {
		return std::make_unique<EngineStrategyMock::Proxy>(strategy_mock);
	};
	auto synch_mock_ptr = tools::make_unique_releasable_raw<system::EngineSynchronizerSlave>(&synch_mock);

	EXPECT_CALL(synch_mock, release());
	EXPECT_THROW(
		std::make_shared<EngineTask>(
			strategy_factory_mock,
			std::move(synch_mock_ptr),
			nullptr
		),
		pisk::infrastructure::NullPointerException
	);
}

TEST(engine_task, exception_when_synch_is_null)
{
	auto synch = make_engine_synchronizer();
	SynchSlaveMock synch_mock(synch->make_slave());
	EngineStrategyMock strategy_mock;
	PatchGateMock patch_gate_mock;

	auto strategy_factory_mock = [&strategy_mock](system::PatchRecipient&) -> system::EngineStrategyPtr {
		return std::make_unique<EngineStrategyMock::Proxy>(strategy_mock);
	};
	auto patch_gate_mock_ptr = std::make_unique<PatchGateMock::Proxy>(patch_gate_mock);

	EXPECT_THROW(
		std::make_shared<EngineTask>(
			strategy_factory_mock,
			nullptr,
			std::move(patch_gate_mock_ptr)
		),
		pisk::infrastructure::NullPointerException
	);
}

TEST(engine_task, exception_when_factory_is_null)
{
	auto synch = make_engine_synchronizer();
	SynchSlaveMock synch_mock(synch->make_slave());
	EngineStrategyMock strategy_mock;
	PatchGateMock patch_gate_mock;

	auto synch_mock_ptr = tools::make_unique_releasable_raw<system::EngineSynchronizerSlave>(&synch_mock);
	auto patch_gate_mock_ptr = std::make_unique<PatchGateMock::Proxy>(patch_gate_mock);

	EXPECT_CALL(synch_mock, release());
	EXPECT_THROW(
		std::make_shared<EngineTask>(
			nullptr,
			std::move(synch_mock_ptr),
			std::move(patch_gate_mock_ptr)
		),
		pisk::infrastructure::NullPointerException
	);
}

TEST(engine_task, exception_when_factory_returns_null)
{
	auto synch = make_engine_synchronizer();
	SynchSlaveMock synch_mock(synch->make_slave());
	EngineStrategyMock strategy_mock;
	PatchGateMock patch_gate_mock;

	auto strategy_factory_mock = [](system::PatchRecipient&) -> system::EngineStrategyPtr {
		return std::unique_ptr<EngineStrategyMock>(nullptr);
	};
	auto synch_mock_ptr = tools::make_unique_releasable_raw<system::EngineSynchronizerSlave>(&synch_mock);
	auto patch_gate_mock_ptr = std::make_unique<PatchGateMock::Proxy>(patch_gate_mock);

	EXPECT_CALL(synch_mock, release());
	EXPECT_THROW(
		std::make_shared<EngineTask>(
			strategy_factory_mock,
			std::move(synch_mock_ptr),
			std::move(patch_gate_mock_ptr)
		),
		pisk::infrastructure::NullPointerException
	);
}

