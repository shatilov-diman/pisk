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
#include <pisk/tools/Job.h>

#include <atomic>
#include <chrono>

using namespace igloo;
using namespace pisk::tools;

Describe(SimplJobTest) {
	It(always_wait_for_job) {
		std::atomic_int x(0);
		{
			ScopedJob job([&](const ScopedJob&) {
				for (int i = 0; i < 10000; ++i)
					++x;
			});
		}
		Assert::That(static_cast<int>(x), Is().EqualTo(10000));
	}
	It(is_stopped_called_when_job_destroing) {
		std::atomic_int x(0);
		{
			ScopedJob job([&x](const ScopedJob& job) {
				while (job.not_stopped())
					++x;
			});
			//get chance for ++x
			while (x == 0)
				std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}
		Assert::That(static_cast<int>(x), Is().GreaterThan(0));
	}
	It(thrown_exception_if_argument_is_nullptr) {
		AssertThrowsEx(
			pisk::infrastructure::InvalidArgumentException,
			ScopedJob(nullptr)
		);
	}
};

Describe(CyclicalScopedJobTest) {
	It(first_lambda_called_in_loop) {
		std::atomic_int x(0);
		{
			CyclicalScopedJob job([&](const CyclicalScopedJob&) {
				++x;
			});
			while (x < 2)
				std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}
		Assert::That(static_cast<int>(x), Is().GreaterThan(0));
	}
	It(second_arg_called_once_after_stop_the_job) {
		std::atomic_int x(0);
		{
			CyclicalScopedJob job(
				[&](const CyclicalScopedJob&) {
				},
				[&] () {
					if (x == 0)
						x = 3;
					else
						x = -3;//check that, second called once
				}
			);
		}
		Assert::That(static_cast<int>(x), Is().EqualTo(3));
	}
	It(third_arg_called_once_before_start_the_job) {
		std::atomic_int x(0);
		{
			CyclicalScopedJob job(
				[&](const CyclicalScopedJob&) {},
				nullptr,
				[&] () {
					if (x == 0)
						x = 1;
					else
						x = -1;//check that, second called once
					return true;
				}
			);
		}
		Assert::That(static_cast<int>(x), Is().EqualTo(1));
	}
	It(iteration_called_at_least_once) {
		std::atomic_bool stopped_while_run(0);
		{
			CyclicalScopedJob job(
				[&stopped_while_run](const CyclicalScopedJob& job) {
					stopped_while_run = job.is_stopped();
				},
				nullptr,
				[&]() {
					//wait, while main thread join our thread
					std::this_thread::sleep_for(std::chrono::milliseconds(100));
					return true;
				}
			);
		}
		Assert::That(static_cast<bool>(stopped_while_run), Is().EqualTo(true));
	}
	It(iteration_and_deinit_is_not_called_if_init_returns_false) {
		std::atomic_int x(0);
		{
			CyclicalScopedJob job(
				[&](const CyclicalScopedJob&) {
					x = x + 1;
				},
				[&]() {
					x = x + 1;
				},
				[&]() {
					//wait, while main thread join our thread
					std::this_thread::sleep_for(std::chrono::milliseconds(100));
					x = x + 1;
					return false;
				}
			);
		}
		Assert::That(static_cast<int>(x), Is().EqualTo(1));
	}
	It(thrown_exception_if_first_argument_is_nullptr) {
		AssertThrowsEx(
			pisk::infrastructure::InvalidArgumentException,
			CyclicalScopedJob(nullptr)
		);
	}
};

