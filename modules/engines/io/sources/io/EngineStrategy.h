// Project pisk
// Copyright (C) 2016-2017 Dmitry Shatilov
//
// This file is a part of the module io of the project pisk.
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

#include <pisk/utils/enum_iterator.h>
#include <pisk/infrastructure/Logger.h>

#include <pisk/system/EngineStrategy.h>
#include <pisk/model/ReflectedKeyboardEvent.h>

#include <mutex>

#include "Keyboard.h"
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
		KeyboardEventDerivativeBuilder keyboard_derivatives;

		std::deque<utils::auto_unsubscriber> subscriptions;

 	public:
		EngineStrategy(const KeyboardPtr& keyboard, system::PatchRecipient& patch_recipient):
			system::EngineStrategyBase(patch_recipient),
			keyboard(keyboard)
		{
			subscribe_keyboard();
		}

		void subscribe_keyboard()
		{
			if (keyboard == nullptr)
				return;

			subscriptions.emplace_back(keyboard->down.subscribe([this](const int key) {
				pisk::infrastructure::Logger::get().debug("io", "Key pressed: %d", key);
				this->get_keyboard_derivatives().on_key_pressed(key);
				this->on_compose(model::io::keyboard_action::pressed, key);
			}));
			subscriptions.emplace_back(keyboard->up.subscribe([this](const int key) {
				pisk::infrastructure::Logger::get().debug("io", "Key released: %d", key);
				this->get_keyboard_derivatives().on_key_released(key);
				this->on_compose(model::io::keyboard_action::released, key);
			}));

			for (const auto& action : utils::iterators::range_all<model::io::keyboard_action>())
			{
				auto& signaler = get_keyboard_derivatives().get_signaler(action);

				subscriptions.emplace_back(signaler.subscribe([this, action](const auto& event) {
					this->on_compose(event.action, event.key);
				}));
			}
		}

		void on_compose(const model::io::keyboard_action& action, const int key)
		{
			system::Patch patch = model::io::ReflectedKeyboardEvent::make_patch(action, key);
			push_event(std::move(patch));
		}
	private:
		virtual Configure on_init_app() final override
		{
			return {};
		}

		virtual void on_deinit_app() final override
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

