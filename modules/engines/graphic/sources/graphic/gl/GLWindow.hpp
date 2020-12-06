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

#include <pisk/defines.h>

#include "../GLWindow.h"

#include <memory>
#include <set>

namespace pisk
{
namespace graphic
{
	template <typename TWindowManager, typename TWindow>
	class GLWindowBase :
		public GLWindow
	{
	public:
		using GLDisplayBasePtr = std::unique_ptr<GLDisplayBase<TWindowManager>>;
		using GLContextBasePtr = std::unique_ptr<GLContextBase<TWindowManager>>;

		using TWindowManagerPtr = tools::InterfacePtr<TWindowManager>;
		using TWindowPtr = tools::shared_relesable_ptr<TWindow>;

	private:
		tools::RemoteTaskExecutor tasks;

		TWindowManagerPtr window_manager;
		TWindowPtr window;

		GLDisplayBasePtr display;
		GLContextBasePtr context;

		std::set<utils::auto_unsubscriber> subscription_manager;
		std::set<utils::auto_unsubscriber> subscription_window;

	public:

		GLWindowBase(const os::WindowManagerPtr& win_manager) :
			window_manager(win_manager.cast<TWindowManager>())
		{
			if (window_manager == nullptr)
			{
				logger::critical("gl_window", "Unable to get WindowManagerEx");
				throw infrastructure::LogicErrorException();
			}
			logger::debug("gl_window", "GLWindow {} was made", this);

			tasks.init();
			subscribe();
		}

		virtual ~GLWindowBase()
		{
			detach_display();
			subscription_manager.clear();
			tasks.deinit();
			logger::debug("gl_window", "GLWindow {} was destroied", this);
		}

		void attach_display(GLDisplayBasePtr&& _display)
		{
			display = std::move(_display);
		}

		GLDisplayBasePtr detach_display()
		{
			return std::move(display);
		}

		virtual bool is_ready() const final override
		{
			return context != nullptr and window != nullptr;
		}

		virtual void process_delaied_tasks()
		{
			tasks.execute_remote_tasks();
		}

		virtual void swap_buffers() const final override
		{
			if (context == nullptr)
				return;
			context->swap_buffers();
		}

		virtual os::Size get_window_size() const final override
		{
			if (window == nullptr)
				return {};
			return window->get_window_size();
		}

	public:
		TWindowManagerPtr get_window_manager() const
		{
			return window_manager;
		}

		const GLDisplayBase<TWindowManager>& get_display() const
		{
			return *display;
		}

	private:
		void subscribe()
		{
			subscription_manager.insert(window_manager->window_create.subscribe([this](const os::WindowPtr& wnd) {
				call_from_engine_thread_async([this, wnd]() {
					this->on_create(wnd);
				});
			}));
			subscription_manager.insert(window_manager->window_destroy.subscribe([this](const os::WindowPtr& wnd) {
				call_from_engine_thread_async([this, wnd]() {
					this->on_destroy(wnd);
				});
			}));
		}

		void on_create(const os::WindowPtr& wnd)
		{
			logger::debug("gl_window", "GLWindow {}: on_create", this);
			if (wnd == nullptr)
				throw infrastructure::NullPointerException();
			if (window != nullptr or context != nullptr)
				throw infrastructure::LogicErrorException();
			attach_gl_window(wnd);
		}

		void on_destroy(const os::WindowPtr& wnd)
		{
			logger::debug("gl_window", "GLWindow {}: on_destroy", this);
			if (wnd == nullptr)
				throw infrastructure::NullPointerException();
			if (window == nullptr or context == nullptr)
				throw infrastructure::LogicErrorException();
			detach_gl_window();
		}

		void attach_gl_window(const os::WindowPtr& wnd)
		{
			auto w = std::dynamic_pointer_cast<typename TWindowPtr::element_type>(wnd);

			std::set<utils::auto_unsubscriber> subscriptions;
			subscribe_window(w, subscriptions);
			subscribe_window_default(w, subscriptions);

			if (on_attach_window(w) == false)
			{
				logger::warning("gl_window", "Unable to attach window: {}, GLWindow: {}", wnd.get(), this);
				return;
			}

			logger::info("gl_window", "GL_VENDOR: {}", reinterpret_cast<const char*>(glGetString(GL_VENDOR)));
			logger::info("gl_window", "GL_RENDERER: {}", reinterpret_cast<const char*>(glGetString(GL_RENDERER)));
			logger::info("gl_window", "GL_VERSION: {}", reinterpret_cast<const char*>(glGetString(GL_VERSION)));
			logger::info("gl_window", "GL_SHADING_LANGUAGE_VERSION: {}", reinterpret_cast<const char*>(glGetString(GL_SHADING_LANGUAGE_VERSION)));
			logger::info("gl_window", "GL_EXTENSIONS: {}", reinterpret_cast<const char*>(glGetString(GL_EXTENSIONS)));

			window.swap(w);
			subscription_window.swap(subscriptions);
			this->on_open.emit();
		}

	public:
		void attach_context(GLContextBasePtr&& _context)
		{
			context = std::move(_context);
			context->make_current(window);
		}

		GLContextBasePtr detach_context()
		{
			return std::move(context);
		}

	private:
		virtual void subscribe_window(const TWindowPtr& wnd, std::set<utils::auto_unsubscriber>& subscriptions) = 0;

		void subscribe_window_default(const TWindowPtr& wnd, std::set<utils::auto_unsubscriber>& subscriptions)
		{
			subscriptions.insert(wnd->window_activate.subscribe([this](const os::WindowPtr&) {
				call_from_engine_thread_async([this]() {
					this->on_activate.emit();
				});
			}));
			subscriptions.insert(wnd->window_deactivate.subscribe([this](const os::WindowPtr&) {
				call_from_engine_thread_async([this]() {
					this->on_deactivate.emit();
				});
			}));

			subscriptions.insert(wnd->window_resize.subscribe([this](const os::WindowPtr&) {
				call_from_engine_thread_async([this]() {
					this->on_resize.emit();
				});
			}));
		}

		void detach_gl_window()
		{
			if (window != nullptr)
				this->on_close.emit();
			subscription_window.clear();
			if (static_cast<bool>(context) != static_cast<bool>(window))
				throw infrastructure::LogicErrorException();
			if (window != nullptr)
				on_detach_window(window);
			window.reset();
		}

		virtual bool on_attach_window(const TWindowPtr& wnd) = 0;

		virtual void on_detach_window(const TWindowPtr& wnd) = 0;

	public:
		void call_from_engine_thread_sync(std::function<void ()> handle)
		{
			tasks.push_remote_task_sync(handle);
		}
		void call_from_engine_thread_async(std::function<void ()> handle)
		{
			tasks.push_remote_task_async(handle);
		}
	};
}//namespace graphic
}//namespace pisk

