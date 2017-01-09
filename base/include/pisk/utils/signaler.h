// Project pisk
// Copyright (C) 2016-2017 Dmitry Shatilov
//
// This file is a part of the module base of the project pisk.
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

#include "../defines.h"
#include "noncopyable.h"
#include "algorithm_utils.h"

#include <functional>
#include <algorithm>
#include <iterator>
#include <memory>
#include <deque>
#include <mutex>

namespace pisk
{
namespace utils
{
	class subscribtion : public noncopyable {
	};
	using auto_unsubscriber = std::shared_ptr<subscribtion>;

namespace details {

	template <typename Signaler, typename EventHandler>
	class subscribtion : public utils::subscribtion {
		std::weak_ptr<Signaler> signaler;
		EventHandler handler;

	public:
		subscribtion() = default;
		subscribtion(const subscribtion& sub) = delete;
		subscribtion operator =(subscribtion&&) = delete;

		subscribtion(std::weak_ptr<Signaler> signaler, const EventHandler& handler) :
			signaler(signaler),
			handler(handler)
		{
			subscribe();
		}
		~subscribtion()
		{
			unsubscribe();
		}
	private:
		void subscribe()
		{
			if (handler != nullptr)
				if (auto ptr = signaler.lock())
					(*ptr) += this->handler;
		}
		void unsubscribe()
		{
			if (handler != nullptr)
				if (auto ptr = signaler.lock())
					(*ptr) -= this->handler;
		}
	};

	template <typename TEvent>
	struct eventhandler {
		using type = std::function<void (const TEvent&)>;
	};
	template <>
	struct eventhandler<void> {
		using type = std::function<void ()>;
	};
}

	template <typename TEvent>
	class light_signaler : public noncopyable
	{
	public:
		using eventhandler = typename details::eventhandler<TEvent>::type;

		void subscribe(const eventhandler& handler) noexcept
		{
			handlers.emplace_back(handler);
		}
		void unsubscribe(const eventhandler& handler) noexcept
		{
			for (auto iter = handlers.begin(); iter != handlers.end();)
			{
				if (iter->target_type() == handler.target_type() &&
					iter->template target<eventhandler>() == handler.template target<eventhandler>())
					iter = handlers.erase(iter);
				else
					++iter;
			}
		}
		template <typename T = TEvent>
		void emit(const typename utils::disable_if<std::is_void<T>::value, T>::type& event) const
		{
			for (const eventhandler& handler : handlers)
				handler(event);
		}
		void emit() const
		{
			for (const eventhandler& handler : handlers)
				handler();
		}
		void clear() noexcept
		{
			handlers.clear();
		}
	private:
		std::deque<eventhandler> handlers;
	};

namespace details {
	template <typename TEvent>
	class signaler : public std::enable_shared_from_this<signaler<TEvent>>
	{
		mutable std::mutex guard;
		std::shared_ptr<light_signaler<TEvent>> impl = std::make_shared<light_signaler<TEvent>>();
		std::deque <std::shared_ptr<light_signaler<TEvent>>> signalers;

	public:
		using eventhandler = typename details::eventhandler<TEvent>::type;

		auto_unsubscriber subscribe(const eventhandler& handler) threadsafe noexcept {
			using _subscribtion = details::subscribtion<signaler, eventhandler>;
			return std::make_shared<_subscribtion>(this->shared_from_this(), handler);
		}

		void operator += (const signaler& sign) threadsafe noexcept
		{
			std::unique_lock<std::mutex> lock(guard);
			signalers.push_back(sign.impl);
			std::copy(sign.signalers.begin(), sign.signalers.end(), std::back_inserter(signalers));
		}
		void operator -= (const signaler& sign) threadsafe noexcept
		{
			std::unique_lock<std::mutex> lock(guard);
			auto newend = std::remove(signalers.begin(), signalers.end(), sign.impl);
			for (const auto& signaler : sign.signalers)
				newend = std::remove(signalers.begin(), newend, signaler);
			signalers.erase(newend, signalers.end());
		}
		void operator += (const eventhandler& handler) threadsafe noexcept
		{
			std::unique_lock<std::mutex> lock(guard);
			impl->subscribe(handler);
		}
		void operator -= (const eventhandler& handler) threadsafe noexcept
		{
			std::unique_lock<std::mutex> lock(guard);
			impl->unsubscribe(handler);
		}
		template <typename T = TEvent>
		void emit(const typename utils::disable_if<std::is_void<T>::value, T>::type& event) const threadsafe
		{
			std::unique_lock<std::mutex> lock(guard);
			impl->emit(event);
			for (const auto& sign : signalers)
				sign->emit(event);
		}
		void emit() const threadsafe
		{
			std::unique_lock<std::mutex> lock(guard);
			impl->emit();
			for (const auto& sign : signalers)
				sign->emit();
		}
		void clear() threadsafe noexcept
		{
			std::unique_lock<std::mutex> lock(guard);
			impl->clear();
			signalers.clear();
		}
	};
}

	template <typename TEvent>
	class signaler : public noncopyable
	{
		std::shared_ptr<details::signaler<TEvent>> impl = std::make_shared<details::signaler<TEvent>>();
	public:

		using eventhandler = typename details::signaler<TEvent>::eventhandler;

		auto_unsubscriber subscribe(const eventhandler& handler) threadsafe noexcept {
			return impl->subscribe(handler);
		}

		void operator += (const signaler& sign) threadsafe noexcept {
			impl->operator += (*sign.impl);
		}
		void operator -= (const signaler& sign) threadsafe noexcept {
			impl->operator -= (*sign.impl);
		}
		void operator += (const eventhandler& handler) threadsafe noexcept {
			impl->operator += (handler);
		}
		void operator -= (const eventhandler& handler) threadsafe noexcept {
			impl->operator -= (handler);
		}
		template <typename T = TEvent>
		void emit(const typename utils::disable_if<std::is_void<T>::value, T>::type& event) const threadsafe {
			return impl->emit(event);
		}
		void emit() const threadsafe {
			return impl->emit();
		}
		void clear() threadsafe noexcept
		{
			return impl->clear();
		}
	};
}
}

