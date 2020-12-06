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

#include "../defines.h"
#include "noncopyable.h"
#include "algorithm_utils.h"

#include <functional>
#include <algorithm>
#include <iterator>
#include <memory>
#include <deque>
#include <mutex>
#include <set>

namespace pisk
{
namespace utils
{
	class subscribtion : public noncopyable {
	};
	using auto_unsubscriber = std::shared_ptr<subscribtion>;

	template<typename base>
	class subscribtions_holder_proxy :
		public base
	{
		std::deque<utils::auto_unsubscriber> subscribtions;

	public:
		template <typename ... TArgs>
		subscribtions_holder_proxy(TArgs&& ... args):
			base(std::forward<TArgs>(args)...)
		{}

		void store_subscribtion(utils::auto_unsubscriber&& subscribtion)
		{
			subscribtions.emplace_back(std::move(subscribtion));
		}
	};

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
			for (auto iter = handlers.begin(); iter != handlers.end(); ++iter)
			{
				if (iter->target_type() == handler.target_type() &&
					iter->template target<eventhandler>() == handler.template target<eventhandler>())
					*iter = nullptr;
			}
		}
		template <typename T = TEvent, typename E = typename utils::disable_if<std::is_void<T>::value, T>::type>
		void emit(const E& event) const
		{
			for (const eventhandler& handler : handlers)
				if (handler != nullptr)
					handler(event);
		}
		template <typename T = TEvent, typename R = std::enable_if_t<std::is_void<T>::value>>
		R emit() const
		{
			for (const eventhandler& handler : handlers)
				if (handler != nullptr)
					handler();
		}
		template <typename T = TEvent, typename E = typename utils::disable_if<std::is_void<T>::value, T>::type>
		void remit(const E& event) const
		{
			for (const eventhandler& handler : iterators::backwards(handlers))
				if (handler != nullptr)
					handler(event);
		}
		template <typename T = TEvent, typename R = std::enable_if_t<std::is_void<T>::value>>
		R remit() const
		{
			for (const eventhandler& handler : iterators::backwards(handlers))
				if (handler != nullptr)
					handler();
		}
		void clear() noexcept
		{
			for (auto iter = handlers.begin(); iter != handlers.end(); ++iter)
				*iter = nullptr;
		}
		void clean() noexcept
		{
			auto newend = std::remove(handlers.begin(), handlers.end(), nullptr);
			handlers.erase(newend, handlers.end());
		}

	private:
		std::deque<eventhandler> handlers;
	};

namespace details {
	template <typename TEvent>
	class signaler : public std::enable_shared_from_this<signaler<TEvent>>
	{
	public:
		using signalerptr = std::shared_ptr<signaler<TEvent>>;
		using eventhandler = typename details::eventhandler<TEvent>::type;

		auto_unsubscriber subscribe(const eventhandler& handler) threadsafe noexcept {
			using _subscribtion = details::subscribtion<signaler, eventhandler>;
			return std::make_shared<_subscribtion>(this->shared_from_this(), handler);
		}

		void operator += (const signalerptr& sign) threadsafe noexcept
		{
			std::lock_guard<std::recursive_mutex> lock(guard);
			signalers.insert(sign);
		}
		void operator -= (const signalerptr& sign) threadsafe noexcept
		{
			std::lock_guard<std::recursive_mutex> lock(guard);
			signalers.erase(sign);
		}
		void operator += (const eventhandler& handler) threadsafe noexcept
		{
			std::lock_guard<std::recursive_mutex> lock(guard);
			impl.subscribe(handler);
		}
		void operator -= (const eventhandler& handler) threadsafe noexcept
		{
			std::lock_guard<std::recursive_mutex> lock(guard);
			impl.unsubscribe(handler);
		}
		template <typename T = TEvent, typename E = typename utils::disable_if<std::is_void<T>::value, T>::type>
		void emit(const E& event) const threadsafe
		{
			std::lock_guard<std::recursive_mutex> lock(guard);
			impl.emit(event);
			auto copy_signalers = signalers;
			for (const auto& signaler : copy_signalers)
				if (signalers.find(signaler) != signalers.end())
					signaler->emit(event);
		}
		template <typename T = TEvent, typename R = std::enable_if_t<std::is_void<T>::value>>
		R emit() const threadsafe
		{
			std::lock_guard<std::recursive_mutex> lock(guard);
			impl.emit();
			auto copy_signalers = signalers;
			for (const auto& signaler : copy_signalers)
				if (signalers.find(signaler) != signalers.end())
					signaler->emit();
		}
		template <typename T = TEvent, typename E = typename utils::disable_if<std::is_void<T>::value, T>::type>
		void remit(const E& event) const threadsafe
		{
			std::lock_guard<std::recursive_mutex> lock(guard);
			auto copy_signalers = signalers;
			for (const auto& signaler : iterators::backwards(copy_signalers))
				if (signalers.find(signaler) != signalers.end())
					signaler->remit(event);
			impl.remit(event);
		}
		template <typename T = TEvent, typename R = std::enable_if_t<std::is_void<T>::value>>
		R remit() const threadsafe
		{
			std::lock_guard<std::recursive_mutex> lock(guard);
			auto copy_signalers = signalers;
			for (const auto& signaler : iterators::backwards(copy_signalers))
				if (signalers.find(signaler) != signalers.end())
					signaler->remit();
			impl.remit();
		}
		void clear() threadsafe noexcept
		{
			std::lock_guard<std::recursive_mutex> lock(guard);
			impl.clear();
			signalers.clear();
		}

	private:

		mutable std::recursive_mutex guard;
		light_signaler<TEvent> impl;
		std::set <signalerptr> signalers;
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
			impl->operator += (sign.impl);
		}
		void operator -= (const signaler& sign) threadsafe noexcept {
			impl->operator -= (sign.impl);
		}
		void operator += (const eventhandler& handler) threadsafe noexcept {
			impl->operator += (handler);
		}
		void operator -= (const eventhandler& handler) threadsafe noexcept {
			impl->operator -= (handler);
		}
		template <typename T = TEvent, typename E = typename utils::disable_if<std::is_void<T>::value, T>::type>
		void emit(const E& event) const threadsafe {
			return impl->emit(event);
		}
		template <typename T = TEvent, typename R = std::enable_if_t<std::is_void<T>::value>>
		R emit() const threadsafe {
			return impl->emit();
		}
		template <typename T = TEvent, typename E = typename utils::disable_if<std::is_void<T>::value, T>::type>
		void remit(const E& event) const threadsafe {
			return impl->remit(event);
		}
		template <typename T = TEvent, typename R = std::enable_if_t<std::is_void<T>::value>>
		R remit() const threadsafe {
			return impl->remit();
		}
		void clear() threadsafe noexcept {
			return impl->clear();
		}
	};
}
}

