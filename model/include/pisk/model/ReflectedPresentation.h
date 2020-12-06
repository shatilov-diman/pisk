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
#include "ReflectedItemRange.h"

namespace pisk
{
namespace model
{
	template <typename cv_property = utils::property>
	class ReflectedPresentationBase:
		public ReflectedItemBase<cv_property>
	{
	public:
		ReflectedPresentationBase(const cv_property& orig, cv_property& prop):
			ReflectedItemBase<cv_property>(orig, prop)
		{}
		ReflectedPresentationBase<const cv_property> const_ref() const
		{
			return ReflectedPresentationBase<const cv_property>(this->orig, this->prop);
		}
		operator ReflectedPresentationBase<const cv_property>() const
		{
			return const_ref();
		}

		ReflectedItemBase<cv_property> properties()
		{
			static const utils::keystring kproperties("properties");
			return this->get_dictionary_item(kproperties);
		}
		ReflectedItemBase<const cv_property> properties() const
		{
			return const_ref().property();
		}

		template <typename State>
		State state(const utils::keystring& id)
		{
			static const utils::keystring kstates("states");
			return this->get_dictionary_item(kstates).template get_custom_item<State>(id);
		}
		template <typename State>
		State state(const utils::keystring& id) const
		{
			return const_ref().template state<State>(id);
		}

		template <typename State>
		ReflectedItemRange<State, cv_property> states()
		{
			static const utils::keystring kstates("states");
			return ReflectedItemRange<State, cv_property>(this->orig[kstates], this->prop[kstates]);
		}
		template <typename State>
		ReflectedItemRange<State, const cv_property> states() const
		{
			return const_ref().states();
		}
	};
	using ConstReflectedPresentation = ReflectedPresentationBase<const utils::property>;
	using ReflectedPresentation = ReflectedPresentationBase<utils::property>;
}
}

