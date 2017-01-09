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


#include <pisk/defines.h>
#include <pisk/infrastructure/Logger.h>
#include <pisk/tools/ComponentsLoader.h>

#include <pisk/tools/MainLoop.h>

#include <pisk/os/SysEvent.h>
#include <pisk/os/android/jni_os_headers.h>
#include <pisk/os/android/os_app_instance.h>

#include <chrono>
#include <thread>
#include <atomic>

namespace pisk
{
namespace os
{
namespace impl
{
	class ALooperWrapper final
	{
		ALooper* looper;

	public:
		ALooperWrapper()
		{
			looper = ALooper_forThread();
			if (looper == nullptr)
				looper = ALooper_prepare(0);
			ALooper_acquire(looper);
		}

		~ALooperWrapper()
		{
			ALooper_release(looper);
		}

		void wake()
		{
			ALooper_wake(looper);
		}
	};

	class MainLoop final :
		public tools::MainLoop
	{
		std::shared_ptr<ALooperWrapper> looper;
		android_app* application;
		std::atomic_bool loop;

	public:
		explicit MainLoop(android_app* app):
			looper(nullptr),
			application(app),
			loop(true)
		{
			if (application == nullptr)
				throw infrastructure::LogicErrorException();
		}

	private:
		virtual void run() final override
		{
			infrastructure::Logger::get().debug("loop", "Begin main loop");

			if (looper != nullptr)
				throw infrastructure::LogicErrorException();

			auto local_looper = std::make_shared<ALooperWrapper>();
			looper = local_looper;

			while (static_cast<bool>(loop))
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(10));
				processes();
			}

			infrastructure::Logger::get().debug("loop", "End main loop");
		}
		virtual void stop() final override
		{
			loop = false;
			if (auto local_looper = looper)
				local_looper->wake();
		}
		virtual void release() final override
		{
			delete this;
		}

		void processes()
		{
			int ident = 0;
			int events = 0;
			struct android_poll_source* source = nullptr;

			while ((ident = ALooper_pollAll(0, nullptr, &events, reinterpret_cast<void**>(&source))) >= 0)
			{
				if (source != nullptr)
					source->process(application, source);

				if (application->destroyRequested != 0)
					return stop();

				if (not loop)
					return;
			}
		}
	};
}
}
}

pisk::tools::SafeComponentPtr __cdecl main_loop_factory(const pisk::tools::ServiceRegistry& temp_sl, const pisk::tools::InstanceFactory& factory, const pisk::utils::property&)
{
	static_assert(std::is_convertible<decltype(&main_loop_factory), pisk::tools::components::ComponentFactory>::value, "Signature was changed!");

	auto dispatcher = temp_sl.get<pisk::os::OSSysEventDispatcher>();
	if (dispatcher == nullptr)
	{
		pisk::infrastructure::Logger::get().critical("loop_factory", "Unable to locate SysEventDispatcher");
		throw pisk::infrastructure::NullPointerException();
	}
	auto app = temp_sl.get<pisk::os::android::OsAppInstance>();
	if (app == nullptr)
	{
		pisk::infrastructure::Logger::get().critical("loop_factory", "Unable to locate OsAppInstance");
		throw pisk::infrastructure::NullPointerException();
	}

	return factory.make<pisk::os::impl::MainLoop>(app->get_app_instance());
}

