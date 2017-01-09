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


#pragma once

#include <pisk/utils/noncopyable.h>

#include <pisk/infrastructure/Logger.h>

#include <pisk/tools/ComponentPtr.h>

#include <set>

namespace pisk
{
namespace os
{
	template <typename Event>
	class SysEventHandlerBase :
		public utils::noncopyable
	{
		template <typename E>
		friend class SysEventHandler;

		SysEventHandlerBase()
		{}

		~SysEventHandlerBase()
		{}

	public:
		virtual void handle(const Event& event) = 0;
	};

	template <typename Event>
	class SysEventDispatcher :
		public core::Component
	{
		template <typename E>
		friend class SysEventHandler;

		std::set<SysEventHandlerBase<Event>*> handlers;

		virtual void release() final override
		{
			pisk::infrastructure::Logger::get().info("sys_event_dispatcher", "Destroing");
			delete this;
		}

		void subscribe(SysEventHandlerBase<Event>* handler)
		{
			pisk::infrastructure::Logger::get().debug("sys_event_dispatcher", "subscribe 0x%x", handler);
			handlers.insert(handler);
		}

		void unsubscribe(SysEventHandlerBase<Event>* handler)
		{
			pisk::infrastructure::Logger::get().debug("sys_event_dispatcher", "unsubscribe 0x%x", handler);
			handlers.erase(handler);
		}

	public:
		constexpr static const char* uid = "sys_event_dispatcher";

		void dispatch(const Event& event) const
		{
			for (auto handler : handlers)
				handler->handle(event);
		}
	};
	template <typename Event>
	using SysEventDispatcherPtr = tools::InterfacePtr<SysEventDispatcher<Event>>;


	template <typename Event>
	class SysEventHandler :
		public SysEventHandlerBase<Event>
	{
		SysEventDispatcherPtr<Event> dispatcher;
	public:
		explicit SysEventHandler(SysEventDispatcherPtr<Event> dispatcher):
			dispatcher(dispatcher)
		{
			dispatcher->subscribe(this);
		}
		~SysEventHandler()
		{
			dispatcher->unsubscribe(this);
		}
	};
}
}

