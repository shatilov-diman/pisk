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

#include <pisk/model/ReflectedKeyboardEvent.h>

namespace pisk
{
namespace io
{
	class KeyboardEventDerivativeBuilder
	{
	public:
		using KeyboardEvent = pisk::model::io::ConstReflectedKeyboardEvent;
		using keyboard_action = pisk::model::io::keyboard_action;

		struct ActionEvent
		{
			keyboard_action action;
			int key;
		};

	private:
		const std::chrono::milliseconds long_press_time_ms = std::chrono::milliseconds(500);
		const std::chrono::milliseconds doublepress_time_ms = std::chrono::milliseconds(500);

		struct Times
		{
			std::chrono::steady_clock::time_point prev_down_time;
			std::chrono::steady_clock::time_point prev_up_time;
			std::chrono::steady_clock::time_point down_time;
			std::chrono::steady_clock::time_point up_time;
		};

		std::map<keyboard_action, pisk::utils::signaler<ActionEvent>> signalers;
		std::map<int, Times> time_points;
		Times common_times;

		std::size_t press_counter;
		int last_released_key_for_single_press;
		bool last_event_can_be_single_press;
	public:

		KeyboardEventDerivativeBuilder():
			press_counter(0),
			last_released_key_for_single_press(0),
			last_event_can_be_single_press(false)
		{}

		pisk::utils::signaler<ActionEvent>& get_signaler(const keyboard_action action)
		{
			return signalers[action];
		}

		void update()
		{
			process_single_press();
		}

		void on_key_pressed(const int key)
		{
			auto& times = time_points[key];
			update_down_time(times);

			++press_counter;
		}
		void on_key_released(const int key)
		{
			auto& times = time_points[key];
			update_up_time(times);

			const auto& double_press_duration = times.up_time - times.prev_down_time;
			const auto& press_duration = times.up_time - times.down_time;

			if (double_press_duration < doublepress_time_ms)
			{
				last_event_can_be_single_press = false;

				ActionEvent event{keyboard_action::double_press, key};
				signalers[event.action].emit(event);
			}
			else if (press_duration < long_press_time_ms)
			{
				last_event_can_be_single_press = press_counter == 1;

				ActionEvent event{keyboard_action::short_press, key};
				signalers[event.action].emit(event);
			}
			else
			{
				last_event_can_be_single_press = false;

				ActionEvent event{keyboard_action::long_press, key};
				signalers[event.action].emit(event);
			}
			--press_counter;
			last_released_key_for_single_press = key;
		}

	private:
		void update_down_time(Times& times)
		{
			times.prev_down_time = times.down_time;
			times.down_time = std::chrono::steady_clock::now();

			common_times.prev_down_time = common_times.down_time;
			common_times.down_time = times.down_time;
		}
		void update_up_time(Times& times)
		{
			times.prev_up_time = times.up_time;
			times.up_time = std::chrono::steady_clock::now();

			common_times.prev_up_time = common_times.up_time;
			common_times.up_time = times.up_time;
		}

		void process_single_press()
		{
			if (not last_event_can_be_single_press)
				return;

			const auto& prev_duration = common_times.down_time - common_times.prev_up_time;
			const auto& duration = std::chrono::steady_clock::now() - common_times.up_time;
			if (duration > doublepress_time_ms and prev_duration > doublepress_time_ms)
			{
				last_event_can_be_single_press = false;

				ActionEvent event{keyboard_action::single_press, last_released_key_for_single_press};
				signalers[event.action].emit(event);
			}
		}
	};
}
}

