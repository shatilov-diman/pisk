// Project pisk
// Copyright (C) 2016-2017 Dmitry Shatilov
//
// This file is a part of the module os of the project pisk.
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


#include <csignal>
#include <stdio.h>
#include <stdlib.h>

#include <cxxabi.h>
#include <dlfcn.h>
#include <unwind.h>

#include <sys/resource.h>

#include <pisk/os/android/jni_os_headers.h>

namespace pisk
{
namespace os
{
namespace impl
{

static void my_fatal_log(const char* message)
{
	static const char log_tag[] = "com.sample.hello_android";

	__android_log_print(ANDROID_LOG_FATAL, log_tag, message);
}

template <typename TArg, typename ... TArgs>
static void my_fatal_log(const char* format, const TArg& arg, const TArgs& ... args)
{
	thread_local char buffer[1024];

	snprintf(buffer, sizeof(buffer), format, arg);

	my_fatal_log(buffer, args ...);
}



struct android_backtrace_state
{
	void ** current;
	void ** end;
};

static _Unwind_Reason_Code android_unwind_callback(struct _Unwind_Context* context, void* arg)
{
	android_backtrace_state* state = static_cast<android_backtrace_state*>(arg);
	const auto pc = _Unwind_GetIP(context);
	if (pc)
	{
		if (state->current == state->end)
			return _URC_END_OF_STACK;

		*state->current++ = reinterpret_cast<void*>(pc);
	}
	return _URC_NO_REASON;
}

static void dump_function(const void* addr, const int index)
{
	const char* fname = "unknown library";
	const char* symbol = "unknown symbol";
	std::ptrdiff_t offset = 0;

	Dl_info info;
	if (dladdr(addr, &info) && info.dli_sname)
	{
		fname = info.dli_fname;
		symbol = info.dli_sname;
		offset = (std::ptrdiff_t)addr - (std::ptrdiff_t)info.dli_fbase;
	}

	int status = 0;
	char* demangled = __cxxabiv1::__cxa_demangle(symbol, 0, 0, &status);

	const char* function = (demangled != nullptr and status == 0) ? demangled : symbol;
	my_fatal_log("%03d: %s [0x%p:0x%x] %s", index, fname, addr, offset, function);
	if (demangled != nullptr)
		free(demangled);
}

static void dump_stack()
{
	my_fatal_log("android stack dump:");

	void* buffer[128];

	android_backtrace_state state;
	state.current = buffer;
	state.end = &buffer[127];

	_Unwind_Backtrace(android_unwind_callback, &state);

	const std::size_t count = static_cast<std::size_t>(state.current - buffer);
	for (std::size_t index = 0; index < count; ++index)
	{
		const void* addr = buffer[index];
		dump_function(addr, index);
	}

	my_fatal_log("android stack dump done");
}

static void print_stack_usage()
{
	rlimit limit;
	getrlimit(RLIMIT_STACK, &limit);

	rusage usage;
	getrusage(RUSAGE_THREAD, &usage);

	const thread_local std::size_t stack_top = reinterpret_cast<std::size_t>(&usage);
	std::size_t current_stack = reinterpret_cast<std::size_t>(&usage);

	my_fatal_log("Stack usage %ld/%ld of %ld/%ld", usage.ru_isrss, stack_top - current_stack, limit.rlim_cur, limit.rlim_max);
}

static void print_pthread_stack_data()
{
	const auto thread = pthread_self();
	pthread_attr_t attributes;
	pthread_getattr_np(thread, &attributes);

	void* stack_base = nullptr;
	std::size_t stack_size = 0;
	pthread_attr_getstack(&attributes, &stack_base, &stack_size);

	my_fatal_log("Thread: %u, stack base: 0x%x, stack size: 0x%x", thread, stack_base, stack_size);
}

static void signal_handler(int signal)
{
	my_fatal_log("Signal received: %d", signal);

	print_stack_usage();
	print_pthread_stack_data();
	dump_stack();
	std::abort();
}

static void at_exit_hook()
{
	my_fatal_log("At exit called");
	dump_stack();
}

void init_dumper()
{
	atexit(impl::at_exit_hook);

	std::signal(SIGABRT, &impl::signal_handler);
	std::signal(SIGFPE, &impl::signal_handler);
	std::signal(SIGILL, &impl::signal_handler);
	std::signal(SIGINT, &impl::signal_handler);
	std::signal(SIGSEGV, &impl::signal_handler);
	std::signal(SIGTERM, &impl::signal_handler);
}

}//namespace impl
}//namespace os
}//namespace pisk

