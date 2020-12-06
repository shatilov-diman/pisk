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

#include <pisk/utils/enum_iterator.h>
#include <pisk/infrastructure/Logger.h>

#include <pisk/system/EngineStrategy.h>
#include <pisk/model/ReflectedKeyboardEvent.h>
#include <pisk/model/ReflectedMouseEvent.h>

#include <mutex>

#include "Keyboard.h"
#include "Mouse.h"

#include "KeyboardEventDerivativeBuilder.h"

namespace pisk { namespace utils { namespace iterators
{
	template<>
	inline iterable_enum_wrapper<model::io::keyboard_action> range_all()
	{
		return {model::io::keyboard_action::released, model::io::keyboard_action::double_press};
	}
}}}

namespace pisk
{
namespace io
{
	class EngineStrategy :
		public system::EngineStrategyBase
	{
		KeyboardPtr keyboard;
		MousePtr mouse;

		KeyboardEventDerivativeBuilder keyboard_derivatives;

		std::deque<utils::auto_unsubscriber> subscriptions;

 	public:
		EngineStrategy(const KeyboardPtr& keyboard, const MousePtr& mouse, system::PatchRecipient& patch_recipient):
			system::EngineStrategyBase(patch_recipient),
			keyboard(keyboard),
			mouse(mouse)
		{
			subscribe_keyboard();
			subscribe_mouse();
		}

		void subscribe_keyboard()
		{
			if (keyboard == nullptr)
				return;

			subscriptions.emplace_back(keyboard->down.subscribe([this](const int key) {
				pisk::logger::debug("io", "Key pressed: {}", key);
				this->get_keyboard_derivatives().on_key_pressed(key);
				this->on_compose(model::io::keyboard_action::pressed, key);
			}));
			subscriptions.emplace_back(keyboard->up.subscribe([this](const int key) {
				pisk::logger::debug("io", "Key released: {}", key);
				this->get_keyboard_derivatives().on_key_released(key);
				this->on_compose(model::io::keyboard_action::released, key);
			}));

			for (const auto& action : utils::iterators::range_all<model::io::keyboard_action>())
			{
				auto& signaler = get_keyboard_derivatives().get_signaler(action);

				subscriptions.emplace_back(signaler.subscribe([this](const auto& event) {
					this->on_compose(event.action, event.key);
				}));
			}
		}

		void on_compose(const model::io::keyboard_action& action, const int key)
		{
			system::Patch patch = model::io::ReflectedKeyboardEvent::make_patch(action, key);
			push_event(std::move(patch));
		}

		void subscribe_mouse()
		{
			if (mouse == nullptr)
				return;

			subscriptions.emplace_back(mouse->up.subscribe([this](const Mouse::ButtonType& button_type) {
				pisk::logger::debug("io", "Mouse button released: {}", button_type);

				static const utils::keystring pressed("pressed");
				system::Patch patch;
				model::io::ReflectedMouseButtonEvent event(utils::property::none_property(), patch);
				event.action() = pressed;
				event.button() = get_button(button_type);
				push_event(std::move(patch));
			}));
			subscriptions.emplace_back(mouse->down.subscribe([this](const Mouse::ButtonType& button_type) {
				pisk::logger::debug("io", "Mouse button pressed: {}", button_type);

				static const utils::keystring released("released");
				system::Patch patch;
				model::io::ReflectedMouseButtonEvent event(utils::property::none_property(), patch);
				event.action() = released;
				event.button() = get_button(button_type);
				push_event(std::move(patch));
			}));
			subscriptions.emplace_back(mouse->wheel.subscribe([this](const Mouse::WheelSide& wheel_side) {
				pisk::logger::debug("io", "Mouse wheel rolled: {}", wheel_side);
				system::Patch patch;
				model::io::ReflectedMouseWheelEvent event(utils::property::none_property(), patch);
				event.wheel() = get_wheel(wheel_side);
				push_event(std::move(patch));
			}));
			subscriptions.emplace_back(mouse->move.subscribe([this](const Mouse::Shift& shift) {
				pisk::logger::debug("io", "Mouse moved: ({}, {})", shift.dx, shift.dy);
				system::Patch patch;
				model::io::ReflectedMouseMoveEvent event(utils::property::none_property(), patch);
				event.x() = shift.dx;
				event.y() = shift.dy;
				push_event(std::move(patch));
			}));
		}

		static utils::keystring get_button(const Mouse::ButtonType& button)
		{
			switch (button)
			{
			case Mouse::ButtonType::Left: return model::io::ReflectedMouseButtonEvent::lbutton();
			case Mouse::ButtonType::Middle: return model::io::ReflectedMouseButtonEvent::mbutton();
			case Mouse::ButtonType::Right: return model::io::ReflectedMouseButtonEvent::rbutton();
			default: return "unknown";
			}
		}
		static utils::keystring get_wheel(const Mouse::WheelSide& wheel)
		{
			switch (wheel)
			{
			case Mouse::WheelSide::Up: return model::io::ReflectedMouseWheelEvent::up_wheel();
			case Mouse::WheelSide::Down: return model::io::ReflectedMouseWheelEvent::down_wheel();
			default: return "unknown";
			}
		}

	private:
		virtual Configure on_init_app() final override
		{
			return {};
		}

		virtual void on_deinit_app() final override
		{}

		virtual void prepatch() final override
		{}

		virtual void patch_scene(const system::PatchPtr&) final override
		{}

		virtual void update() final override
		{
			get_keyboard_derivatives().update();
		}

	private:
		KeyboardEventDerivativeBuilder& get_keyboard_derivatives()
		{
			return keyboard_derivatives;
		}

		void push_event(system::Patch&& event)
		{
			system::Patch out;
			out["events"][std::size_t(0)] = event;
			push_changes(std::move(out));
		}
	};
}
}

