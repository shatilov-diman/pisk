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

#include "defines.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

namespace _details
{
	using namespace testing;

	class ForwarderEventListener :
		public ::testing::TestEventListener
	{
		::testing::TestEventListener* origin;
	public:
		ForwarderEventListener(::testing::TestEventListener* origin) :
			origin(origin)
		{}

		virtual void OnTestProgramStart(const UnitTest& unit_test) override
		{origin->OnTestProgramStart(unit_test);}
		virtual void OnTestIterationStart(const UnitTest& unit_test, int iteration) override
		{origin->OnTestIterationStart(unit_test, iteration);}
		virtual void OnEnvironmentsSetUpStart(const UnitTest& unit_test) override
		{origin->OnEnvironmentsSetUpStart(unit_test);}
		virtual void OnEnvironmentsSetUpEnd(const UnitTest& unit_test) override
		{origin->OnEnvironmentsSetUpEnd(unit_test);}
		virtual void OnTestCaseStart(const TestCase& test_case) override
		{origin->OnTestCaseStart(test_case);}
		virtual void OnTestStart(const TestInfo& test_info) override
		{origin->OnTestStart(test_info);}
		virtual void OnTestPartResult(const TestPartResult& test_part_result) override
		{origin->OnTestPartResult(test_part_result);}
		virtual void OnTestEnd(const TestInfo& test_info) override
		{origin->OnTestEnd(test_info);}
		virtual void OnTestCaseEnd(const TestCase& test_case) override
		{origin->OnTestCaseEnd(test_case);}
		virtual void OnEnvironmentsTearDownStart(const UnitTest& unit_test) override
		{origin->OnEnvironmentsTearDownStart(unit_test);}
		virtual void OnEnvironmentsTearDownEnd(const UnitTest& unit_test) override
		{origin->OnEnvironmentsTearDownEnd(unit_test);}
		virtual void OnTestIterationEnd(const UnitTest& unit_test, int iteration) override
		{origin->OnTestIterationEnd(unit_test, iteration);}
		virtual void OnTestProgramEnd(const UnitTest& unit_test) override
		{origin->OnTestProgramEnd(unit_test);}
	};
	class DotsFormatEventListener :
		public ForwarderEventListener
	{
		std::string dots;

	public:
		DotsFormatEventListener(::testing::TestEventListener* origin) :
			ForwarderEventListener(origin)
		{}

		virtual void OnTestProgramStart(const UnitTest&) override {}
		virtual void OnTestIterationStart(const UnitTest&, int) override {}
		virtual void OnEnvironmentsSetUpStart(const UnitTest&) override {}
		virtual void OnEnvironmentsSetUpEnd(const UnitTest&) override {}
		virtual void OnTestCaseStart(const TestCase&) override {}
		virtual void OnTestStart(const TestInfo&) override {}
		virtual void OnTestPartResult(const TestPartResult& result) override
		{
			if (result.failed())
				ForwarderEventListener::OnTestPartResult(result);
		}
		virtual void OnTestEnd(const TestInfo& test_info) override
		{
			if (test_info.result()->Passed())
				dots.push_back('.');
			else
				dots.push_back('F');
		}
		virtual void OnTestCaseEnd(const TestCase&) override {}
		virtual void OnEnvironmentsTearDownStart(const UnitTest& unit_test) override
		{
			std::cout << dots << std::endl;
			std::cout
				<< (unit_test.failed_test_count() > 0 ? "Test run fail. " : "Test run complete. ")
				<< unit_test.total_test_count() << " tests run, "
				<< unit_test.successful_test_count() << " succeeded, "
				<< unit_test.failed_test_count() << " failed."
				<< std::endl;
		}
		virtual void OnEnvironmentsTearDownEnd(const UnitTest&) override {}
		virtual void OnTestIterationEnd(const UnitTest&, int) override {}
		virtual void OnTestProgramEnd(const UnitTest&) override {}
	};
}//namespace _details

inline void gtest_set_dots_format()
{
	::testing::TestEventListeners& listeners = ::testing::UnitTest::GetInstance()->listeners();
	::testing::TestEventListener* listener = listeners.Release(listeners.default_result_printer());
	listeners.Append(new _details::DotsFormatEventListener(listener));
}

