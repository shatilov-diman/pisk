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

namespace pisk
{
namespace model
{
namespace io
{
	enum class keyboard_action
	{
		released,
		pressed,
		short_press,
		long_press,
		single_press,
		double_press,
	};
	inline utils::keystring to_keystring(const keyboard_action action)
	{
		static const utils::keystring up("released");
		static const utils::keystring down("pressed");
		static const utils::keystring short_press("short");
		static const utils::keystring long_press("long");
		static const utils::keystring single_press("single");
		static const utils::keystring double_press("double");
		switch (action)
		{
			case keyboard_action::released:
				return up;
			case keyboard_action::pressed:
				return down;
			case keyboard_action::short_press:
				return short_press;
			case keyboard_action::long_press:
				return long_press;
			case keyboard_action::single_press:
				return single_press;
			case keyboard_action::double_press:
				return double_press;

			default:
				throw UnexpectedItemTypeException();
		};
	}

	template <typename cv_property = utils::property>
	class ReflectedKeyboardEventBase :
		public ReflectedEventBase<cv_property>
	{
	public:
		static utils::property make_patch(const keyboard_action action, const int key)
		{
			cv_property patch;
			auto event = ReflectedKeyboardEventBase<>(utils::property::none_property(), patch);
			event.key() = key;
			event.action() = to_keystring(action);
			return patch;
		}
		static const utils::keystring& key_source()
		{
			static const utils::keystring key("keyboard");
			return key;
		}
		static const utils::keystring& key_type()
		{
			static const utils::keystring key("button");
			return key;
		}

		ReflectedKeyboardEventBase(const cv_property& orig, const cv_property& prop):
			ReflectedEventBase<cv_property>(orig, prop)
		{
			if (this->source().as_keystring() != key_source())
				throw UnexpectedItemTypeException();
			if (this->type().as_keystring() != key_type())
				throw UnexpectedItemTypeException();
		}
		template <typename check_type = cv_property, typename = typename std::enable_if<!std::is_const<check_type>::value>::type>
		ReflectedKeyboardEventBase(const cv_property& orig, cv_property& prop):
			ReflectedEventBase<cv_property>(orig, prop)
		{
			this->source() = key_source();
			this->type() = key_type();
		}
		ReflectedKeyboardEventBase<const cv_property> const_ref() const
		{
			return ReflectedKeyboardEventBase<const cv_property>(this->orig, this->prop);
		}
		operator ReflectedKeyboardEventBase<const cv_property>() const
		{
			return const_ref();
		}

		ReflectedItemBase<cv_property> key()
		{
			static const utils::keystring kkey("key");
			return this->get_int_item(kkey);
		}
		ReflectedItemBase<const cv_property> key() const
		{
			return const_ref().key();
		}
	};
	using ConstReflectedKeyboardEvent = ReflectedKeyboardEventBase<const utils::property>;
	using ReflectedKeyboardEvent = ReflectedKeyboardEventBase<utils::property>;
}
}
}

