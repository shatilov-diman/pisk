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

namespace pisk
{
namespace infrastructure
{
	class Exception
	{};

	class NullPointerException : public Exception
	{};

	class InvalidArgumentException : public Exception
	{};

	class LogicErrorException : public Exception
	{};

	class OutOfRangeException : public Exception
	{};

	class OutOfMemoryException : public Exception
	{};

	class InitializeError : public Exception
	{};

	class UnsupportedException : public Exception
	{};
}
}

