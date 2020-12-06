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

namespace pisk
{
namespace model
{
namespace io
{
	static inline const utils::keystring& mouse_source() {
		static const utils::keystring key("mouse");
		return key;
	}

	template <typename cv_property = utils::property>
	class ReflectedMouseButtonEventBase :
		public ReflectedEventBase<cv_property>
	{
	public:
		ReflectedMouseButtonEventBase(const cv_property& orig, const cv_property& prop):
			ReflectedEventBase<cv_property>(orig, prop)
		{
			if (this->source().as_keystring() != mouse_source())
				throw UnexpectedItemTypeException();
			const auto& event_type = this->type().as_keystring();
			if (event_type != button_type())
				throw UnexpectedItemTypeException();
		}
		template <typename check_type = cv_property, typename = typename std::enable_if<!std::is_const<check_type>::value>::type>
		ReflectedMouseButtonEventBase(const cv_property& orig, cv_property& prop):
			ReflectedEventBase<cv_property>(orig, prop)
		{
			this->source() = mouse_source();
			this->type() = button_type();
		}
		ReflectedMouseButtonEventBase<const cv_property> const_ref() const
		{
			return ReflectedMouseButtonEventBase<const cv_property>(this->orig, this->prop);
		}
		operator ReflectedMouseButtonEventBase<const cv_property>() const
		{
			return const_ref();
		}

		static const utils::keystring& button_type() {
			static const utils::keystring key("button");
			return key;
		}
		static const utils::keystring& lbutton() {
			static const utils::keystring key("left");
			return key;
		}
		static const utils::keystring& mbutton() {
			static const utils::keystring key("middle");
			return key;
		}
		static const utils::keystring& rbutton() {
			static const utils::keystring key("right");
			return key;
		}

		ReflectedItemBase<cv_property> button()
		{
			static const utils::keystring k("button");
			return this->get_int_item(k);
		}
		ReflectedItemBase<const cv_property> button() const
		{
			return const_ref().button();
		}
	};
	using ConstReflectedMouseButtonEvent = ReflectedMouseButtonEventBase<const utils::property>;
	using ReflectedMouseButtonEvent = ReflectedMouseButtonEventBase<utils::property>;

	template <typename cv_property = utils::property>
	class ReflectedMouseWheelEventBase :
		public ReflectedEventBase<cv_property>
	{
	public:
		ReflectedMouseWheelEventBase(const cv_property& orig, const cv_property& prop):
			ReflectedEventBase<cv_property>(orig, prop)
		{
			if (this->source().as_keystring() != mouse_source())
				throw UnexpectedItemTypeException();
			const auto& event_type = this->type().as_keystring();
			if (event_type != wheel_type())
				throw UnexpectedItemTypeException();
		}
		template <typename check_type = cv_property, typename = typename std::enable_if<!std::is_const<check_type>::value>::type>
		ReflectedMouseWheelEventBase(const cv_property& orig, cv_property& prop):
			ReflectedEventBase<cv_property>(orig, prop)
		{
			this->source() = mouse_source();
			this->type() = wheel_type();
		}
		ReflectedMouseWheelEventBase<const cv_property> const_ref() const
		{
			return ReflectedMouseWheelEventBase<const cv_property>(this->orig, this->prop);
		}
		operator ReflectedMouseWheelEventBase<const cv_property>() const
		{
			return const_ref();
		}

		static const utils::keystring& wheel_type() {
			static const utils::keystring key("wheel");
			return key;
		}
		static const utils::keystring& up_wheel() {
			static const utils::keystring key("up_wheel");
			return key;
		}
		static const utils::keystring& down_wheel() {
			static const utils::keystring key("down_wheel");
			return key;
		}

		ReflectedItemBase<cv_property> wheel()
		{
			static const utils::keystring k("wheel");
			return this->get_string_item(k);
		}
		ReflectedItemBase<const cv_property> wheel() const
		{
			return const_ref().wheel();
		}
	};
	using ConstReflectedMouseWheelEvent = ReflectedMouseWheelEventBase<const utils::property>;
	using ReflectedMouseWheelEvent = ReflectedMouseWheelEventBase<utils::property>;

	template <typename cv_property = utils::property>
	class ReflectedMouseMoveEventBase :
		public ReflectedEventBase<cv_property>
	{
	public:
		ReflectedMouseMoveEventBase(const cv_property& orig, const cv_property& prop):
			ReflectedEventBase<cv_property>(orig, prop)
		{
			if (this->source().as_keystring() != mouse_source())
				throw UnexpectedItemTypeException();
			const auto& event_type = this->type().as_keystring();
			if (event_type != move_type())
				throw UnexpectedItemTypeException();
		}
		template <typename check_type = cv_property, typename = typename std::enable_if<!std::is_const<check_type>::value>::type>
		ReflectedMouseMoveEventBase(const cv_property& orig, cv_property& prop):
			ReflectedEventBase<cv_property>(orig, prop)
		{
			this->source() = mouse_source();
			this->type() = move_type();
		}
		ReflectedMouseMoveEventBase<const cv_property> const_ref() const
		{
			return ReflectedMouseMoveEventBase<const cv_property>(this->orig, this->prop);
		}
		operator ReflectedMouseMoveEventBase<const cv_property>() const
		{
			return const_ref();
		}

		static const utils::keystring& move_type() {
			static const utils::keystring key("move");
			return key;
		}

		ReflectedItemBase<cv_property> x()
		{
			static const utils::keystring k("x");
			return this->get_int_item(k);
		}
		ReflectedItemBase<const cv_property> x() const
		{
			return const_ref().x();
		}
		ReflectedItemBase<cv_property> y()
		{
			static const utils::keystring k("y");
			return this->get_int_item(k);
		}
		ReflectedItemBase<const cv_property> y() const
		{
			return const_ref().y();
		}
	};
	using ConstReflectedMouseMoveEvent = ReflectedMouseMoveEventBase<const utils::property>;
	using ReflectedMouseMoveEvent = ReflectedMouseMoveEventBase<utils::property>;
}
}
}

