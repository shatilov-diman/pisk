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

#include <pisk/model/ReflectedPresentation.h>

namespace pisk
{
namespace model
{
namespace script
{
	template <typename cv_property>
	class StatePresentationBase:
		public ReflectedItemBase<cv_property>
	{
	public:
		StatePresentationBase(const cv_property& orig, cv_property& prop):
			ReflectedItemBase<cv_property>(orig, prop)
		{}
		StatePresentationBase<const cv_property> const_ref() const
		{
			return StatePresentationBase<const cv_property>(this->orig, this->prop);
		}
		operator StatePresentationBase<const cv_property>() const
		{
			return const_ref();
		}

		ReflectedItemBase<cv_property> res_id()
		{
			static const utils::keystring kresid("res_id");
			return this->get_string_item(kresid);
		}
		ReflectedItemBase<const cv_property> res_id() const
		{
			return const_ref().res_id();
		}

		ReflectedItemBase<cv_property> function()
		{
			static const utils::keystring kfunction("function");
			return this->get_string_item(kfunction);
		}
		ReflectedItemBase<const cv_property> function() const
		{
			return const_ref().function();
		}

		ReflectedItemBase<cv_property> arguments()
		{
			static const utils::keystring karguments("arguments");
			return this->get_item(karguments);
		}
		ReflectedItemBase<const cv_property> arguments() const
		{
			return const_ref().arguments();
		}
	};
	using ConstStatePresentation = StatePresentationBase<const utils::property>;
	using StatePresentation = StatePresentationBase<utils::property>;


	template <typename cv_property>
	class PresentationBase:
		public ReflectedPresentationBase<cv_property>
	{
	public:
		static const utils::keystring presentation_id()
		{
			return "script";
		}

		PresentationBase(const cv_property& orig, cv_property& prop):
			ReflectedPresentationBase<cv_property>(orig, prop)
		{}
		PresentationBase<const cv_property> const_ref() const
		{
			return PresentationBase<const cv_property>(this->orig, this->prop);
		}
		operator PresentationBase<const cv_property>() const
		{
			return const_ref();
		}

		StatePresentationBase<cv_property> state(const utils::keystring& id)
		{
			return ReflectedPresentationBase<cv_property>::template state<StatePresentationBase<cv_property>>(id);
		}
		StatePresentationBase<const cv_property> state(const utils::keystring& id) const
		{
			return const_ref().template state(id);
		}
	};
	using ConstPresentation = PresentationBase<const utils::property>;
	using Presentation = PresentationBase<utils::property>;
}
}
}

