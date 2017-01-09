// Project pisk
// Copyright (C) 2016-2017 Dmitry Shatilov
//
// This file is a part of the module model of the project pisk.
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

#include "ReflectedEvent.h"
#include "ReflectedObject.h"
#include "ReflectedItemRange.h"

namespace pisk
{
namespace model
{
	template <typename cv_property>
	class ReflectedSceneBase:
		public ReflectedObjectBase<cv_property>
	{
	public:
		ReflectedSceneBase(const cv_property& orig, cv_property& prop):
			ReflectedObjectBase<cv_property>(orig, prop)
		{}
		ReflectedSceneBase<const cv_property> const_ref() const
		{
			return ReflectedSceneBase<const cv_property>(this->orig, this->prop);
		}
		operator ReflectedSceneBase<const cv_property>() const
		{
			return const_ref();
		}

		template <typename ReflectedEventType>
		ReflectedEventType push_event()
		{
			this->template check_type<ReflectedEventType, ReflectedEventBase<cv_property>>();

			static const utils::keystring kevents("events");
			const std::size_t newindex = this->prop[kevents].size();
			const std::size_t orig_events_count = this->orig[kevents].size();
			if (newindex < orig_events_count)
				return ReflectedEventBase<cv_property>(this->orig[kevents][newindex], this->prop[kevents][newindex]);
			return ReflectedEventBase<cv_property>(cv_property::none_property(), this->prop[kevents][newindex]);
		}
		ReflectedItemRange<ReflectedEventBase<cv_property>, cv_property> events()
		{
			static const utils::keystring kevents("events");
			return ReflectedItemRange<ReflectedEventBase<cv_property>, cv_property>(this->orig[kevents], this->prop[kevents]);
		}
		ReflectedItemRange<ReflectedEventBase<const cv_property>, const cv_property> events() const
		{
			return const_ref().events();
		}
	};
	using ConstReflectedScene = ReflectedSceneBase<const utils::property>;
	using ReflectedScene = ReflectedSceneBase<utils::property>;
}
}

