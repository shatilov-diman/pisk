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
#include <pisk/gtest.h>

int main(int argc, char* argv[])
{
	::testing::InitGoogleTest(&argc, argv);
	gtest_set_dots_format();
	return RUN_ALL_TESTS() | igloo::TestRunner::RunAllTests(argc, argv);
}

