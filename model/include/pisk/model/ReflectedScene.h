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

