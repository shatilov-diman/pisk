// Project pisk
// Copyright (C) 2016-2017 Dmitry Shatilov
//
// This file is a part of the module base of the project pisk.
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
				}
			);
		}
		Assert::That(static_cast<bool>(stopped_while_run), Is().EqualTo(true));
	}
	It(thrown_exception_if_first_argument_is_nullptr) {
		AssertThrowsEx(
			pisk::infrastructure::InvalidArgumentException,
			CyclicalScopedJob(nullptr)
		);
	}
};

