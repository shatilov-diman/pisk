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

#include "ReflectedItem.h"

namespace pisk
{
namespace model
{
	template <typename cv_property>
	class ReflectedEventBase :
		public ReflectedItemBase<cv_property>
	{
	public:
		ReflectedEventBase(const cv_property& orig, cv_property& prop):
			ReflectedItemBase<cv_property>(orig, prop)
		{}
		ReflectedEventBase<const cv_property> const_ref() const
		{
			return ReflectedEventBase<const cv_property>(this->orig, this->prop);
		}
		operator ReflectedEventBase<const cv_property>() const
		{
			return const_ref();
		}

		template <typename CastedEvent>
		CastedEvent cast()
		{
			this->template check_type<CastedEvent, ReflectedEventBase>();
			return CastedEvent(this->orig, this->prop);
		}
		template <typename CastedEvent>
		CastedEvent cast() const
		{
			return const_ref().template cast<CastedEvent>();
		}

	public:
		ReflectedItemBase<cv_property> source()
		{
			static const utils::keystring ksource("source");
			return this->get_string_item(ksource);
		}
		ReflectedItemBase<const cv_property> source() const
		{
			return const_ref().source();
		}
		ReflectedItemBase<cv_property> type()
		{
			static const utils::keystring ktype("type");
			return this->get_string_item(ktype);
		}
		ReflectedItemBase<const cv_property> type() const
		{
			return const_ref().type();
		}
		ReflectedItemBase<cv_property> action()
		{
			static const utils::keystring kaction("action");
			return this->get_string_item(kaction);
		}
		ReflectedItemBase<const cv_property> action() const
		{
			return const_ref().action();
		}
	};
	using ConstReflectedEvent = ReflectedEventBase<const utils::property>;
	using ReflectedEvent = ReflectedEventBase<utils::property>;
}
}

