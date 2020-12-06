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


#include <pisk/bdd.h>
#include <pisk/tools/RemoteTaskList.h>

#include <atomic>
#include <chrono>
#include <future>

using namespace igloo;
using namespace pisk::tools;
using namespace pisk::tools::_details;

Describe(RemoteTaskListTest) {
	RemoteTaskList list;
	std::size_t exec_id;
	void SetUp() {
		exec_id = 0;
	}
	It(is_empty) {
		RemoteTask task;
		Assert::That(Root().list.pop(task), Is().EqualTo(false));
		Assert::That(task.valid(), Is().EqualTo(false));
	}
	When(push_one_element) {
		void SetUp() {
			Root().list.push([this]() {Root().exec_id = 1; });
		}
		Then(pushed_element_can_be_popped) {
			RemoteTask task;
			Assert::That(Root().list.pop(task), Is().EqualTo(true));
			Assert::That(task.valid(), Is().EqualTo(true));
		}
		When(push_one_element_more) {
			void SetUp() {
				Root().list.push([this]() {Root().exec_id = 2; });
			}
			Then(first_element_will_poped_then_second_then_empty) {
				RemoteTask task;

				Assert::That(Root().list.pop(task), Is().EqualTo(true));
				Assert::That(task.valid(), Is().EqualTo(true));
				task();
				Assert::That(Root().exec_id, Is().EqualTo(1U));

				task.reset();
				Assert::That(Root().list.pop(task), Is().EqualTo(true));
				Assert::That(task.valid(), Is().EqualTo(true));
				task();
				Assert::That(Root().exec_id, Is().EqualTo(2U));

				RemoteTask trash{};
				task.swap(trash);
				Assert::That(Root().list.pop(task), Is().EqualTo(false));
				Assert::That(task.valid(), Is().EqualTo(false));
			}
		};
	};
	When(push_one_element_and_pop_it) {
		void SetUp() {
			Root().list.push([]() {});
			RemoteTask task;
			Assert::That(Root().list.pop(task), Is().EqualTo(true));
		}
		Then(list_is_empty) {
			RemoteTask task;
			Assert::That(Root().list.pop(task), Is().EqualTo(false));
			Assert::That(task.valid(), Is().EqualTo(false));
		}
	};
};

Describe(RemoteTaskExecutorNoInitTest) {
	RemoteTaskExecutor executor;
	It(execute_throw_exception_if_no_init_calls) {
		AssertThrowsEx(pisk::infrastructure::InitializeError, executor.execute_remote_tasks());
	}
	It(check_void_async) {
		AssertThrowsEx(pisk::infrastructure::InitializeError, executor.push_remote_task_async([]() -> void {}));
	}
	It(check_sizet_async) {
		AssertThrowsEx(pisk::infrastructure::InitializeError, executor.push_remote_task_async([]() -> std::size_t {return 0;}));
	}
	It(check_void_sync) {
		AssertThrowsEx(pisk::infrastructure::InitializeError, executor.push_remote_task_sync([]() -> void {}));
	}
	It(check_sizet_sync) {
		AssertThrowsEx(pisk::infrastructure::InitializeError, executor.push_remote_task_sync([]() -> std::size_t {return 0;}));
	}
};

Describe(WrongCalls) {
	RemoteTaskExecutor executor;
	It(deinit_from_another_thread) {
		std::async(std::launch::async, [this]() {executor.init();}).wait();
		AssertThrowsEx(pisk::infrastructure::InitializeError, executor.deinit());
	}
	It(deinit_without_init) {
		AssertThrowsEx(pisk::infrastructure::InitializeError, executor.deinit());
	}
	It(twice_init) {
		executor.init();
		AssertThrowsEx(pisk::infrastructure::InitializeError, executor.init());
	}
	It(twice_deinit) {
		executor.init();
		executor.deinit();
		AssertThrowsEx(pisk::infrastructure::InitializeError, executor.deinit());
	}
};

Describe(RemoteTaskExecutorTest) {
	RemoteTaskExecutor executor;
	std::size_t exec_id;
	void SetUp() {
		exec_id = 0;
		executor.init();
	}
	void TearDown()
	{
		executor.deinit();
	}

	template <typename Waiter>
	void execute_tasks(Waiter& waiter)
	{
		for (std::size_t i = 0; i < 100 && waiter.wait_for(std::chrono::milliseconds(10)) != std::future_status::ready; ++i)
			executor.execute_remote_tasks();
	}

	It(check_void_async) {
		auto result = executor.push_remote_task_async([this]() -> void {Root().exec_id = 1; });
		executor.execute_remote_tasks();
		Assert::That(Root().exec_id, Is().EqualTo(1U));
	}
	It(check_size_t_async) {
		auto result = executor.push_remote_task_async([this]() -> std::size_t {
			Root().exec_id = 1;
			return 5;
		});
		executor.execute_remote_tasks();
		Assert::That(result.get(), Is().EqualTo(5U));
		Assert::That(Root().exec_id, Is().EqualTo(1U));
	}
	It(check_uniqptr_async) {
		auto result = executor.push_remote_task_async([this]() -> std::unique_ptr<std::size_t> {
			Root().exec_id = 1;
			return std::make_unique<std::size_t>(3U);
		});
		executor.execute_remote_tasks();
		Assert::That(*result.get(), Is().EqualTo(3U));
		Assert::That(Root().exec_id, Is().EqualTo(1U));
	}
	It(check_void_sync_another_thread) {
		auto result = std::async(std::launch::async, [this]() {
			return executor.push_remote_task_sync([this]() -> void {Root().exec_id = 1; });
		});
		execute_tasks(result);
		Assert::That(Root().exec_id, Is().EqualTo(1U));
	}
	It(check_size_t_sync_another_thread) {
		auto result = std::async(std::launch::async, [this]() -> std::size_t {
			return executor.push_remote_task_sync([this]() -> std::size_t {
				Root().exec_id = 1;
				return 5;
			});
		});
		execute_tasks(result);
		Assert::That(result.get(), Is().EqualTo(5U));
		Assert::That(Root().exec_id, Is().EqualTo(1U));
	}
	It(check_uniqptr_sync_another_thread) {
		auto result = std::async(std::launch::async, [this]() -> std::unique_ptr<std::size_t> {
			return executor.push_remote_task_sync([this]() -> std::unique_ptr<std::size_t> {
				Root().exec_id = 1;
				return std::make_unique<std::size_t>(3U);
			});
		});
		execute_tasks(result);
		Assert::That(*result.get(), Is().EqualTo(3U));
		Assert::That(Root().exec_id, Is().EqualTo(1U));
	}
	It(check_void_sync) {
		executor.push_remote_task_sync([this]() -> void {Root().exec_id = 1; });
		Assert::That(Root().exec_id, Is().EqualTo(1U));
	}
	It(check_size_t_sync) {
		auto result = executor.push_remote_task_sync([this]() -> std::size_t {
				Root().exec_id = 1;
				return 5U;
		});
		Assert::That(result, Is().EqualTo(5U));
		Assert::That(Root().exec_id, Is().EqualTo(1U));
	}
	It(check_uniqptr_sync) {
		auto result = executor.push_remote_task_sync([this]() -> std::unique_ptr<std::size_t> {
			Root().exec_id = 1;
			return std::make_unique<std::size_t>(3U);
		});
		Assert::That(*result, Is().EqualTo(3U));
		Assert::That(Root().exec_id, Is().EqualTo(1U));
	}
};
