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

#include <pisk/utils/noncopyable.h>

#include <pisk/os/WindowManager.h>

#include <pisk/model/Path.h>
#include <pisk/model/ReflectedScene.h>
#include <pisk/model/location/ReflectedPresentation.h>

#include "Engine.h"
#include "GLWindow.h"
#include "ResourceLoader.h"

#include "gl/opengl.h"

#include <atomic>

namespace pisk
{
namespace graphic
{
	class EngineController :
		public utils::noncopyable
	{
		const pisk::utils::property config;

		os::WindowManagerPtr window_manager;
		ResourceLoaderPtr resource_loader;

		Engine graphic_engine;
		utils::property scene;

		std::set<utils::auto_unsubscriber> subscriptions;
		GLWindowPtr gl_window;

	public:
		utils::signaler<void> on_activate_singaler;
		utils::signaler<void> on_deactivate_singaler;
		utils::signaler<void> on_close_request_signaler;
		utils::signaler<void> on_close_signaler;

		EngineController(const pisk::utils::property& config, const os::WindowManagerPtr& win_manager, ResourceLoaderPtr&& _resource_loader) :
			config(config),
			window_manager(win_manager),
			resource_loader(std::move(_resource_loader))
		{}
		virtual ~EngineController() = default;

	public:
		void on_init_app()
		{
			gl_window = graphic::make_gl_window(window_manager);
			subscribe();
			gl_window->request_window();
		}

		void on_deinit_app()
		{
			gl_window.reset();
			subscriptions.clear();
		}

	private:
		void subscribe()
		{
			subscriptions.insert(gl_window->on_open.subscribe([this]() {
				this->update_view();
			}));
			subscriptions.insert(gl_window->on_close.subscribe([this]() {
				logger::info("opengl", "window is closed");
				this->on_close_signaler.emit();
			}));

			subscriptions.insert(gl_window->on_activate.subscribe([this]() {
				this->on_activate_singaler.emit();
			}));
			subscriptions.insert(gl_window->on_deactivate.subscribe([this]() {
				this->on_deactivate_singaler.emit();
			}));
			subscriptions.insert(gl_window->on_close_request.subscribe([this]() {
				this->on_close_request_signaler.emit();
			}));

			subscriptions.insert(gl_window->on_update.subscribe([this]() {
				this->update_view();
			}));
			subscriptions.insert(gl_window->on_resize.subscribe([this]() {
				this->update_view();
			}));
		}

		void update_view()
		{
			if (not gl_window->is_ready())
				return;

			const auto size = gl_window->get_window_size();
			logger::debug("opengl", "Update Viewport: {}x{}", size.cx, size.cy);
			glViewport(0, 0, size.cx, size.cy);
		}

	public:
		void prepatch()
		{
			gl_window->process_delaied_tasks();
		}

		void update()
		{
			gl_window->process_delaied_tasks();
			if (gl_window->is_ready())
			{
				graphic_engine.update();
				if (eglGetError() != EGL_SUCCESS)
					logger::error("opengl", "Error: {}", egl_get_error());

				gl_window->swap_buffers();
			}
		}

		void apply_changes(const pisk::system::PatchPtr& patch)
		{
			if (patch == nullptr)
				throw infrastructure::NullPointerException();

			if (patch->is_none())
				return clear_model();
			if (not patch->is_dictionary())
				throw pisk::infrastructure::LogicErrorException();

			model::ConstReflectedScene scene_object(scene, *patch);
			update_scene(scene_object);
			utils::property::replace(scene, *patch);
		}

	private:
		//TODO: const reference?
		void update_scene(pisk::model::ConstReflectedScene& scene_object)
		{
			for (const auto& event : scene_object.events())
			{
				process_event(event);
			}
			walk(scene_object, {});
		}

		void process_event(const pisk::model::ConstReflectedEvent& event)
		{
			logger::debug("opengl", "Event received; source: {}; type: {}; action: {}", event.source().as_keystring().c_str(), event.type().as_keystring().c_str(), event.action().as_keystring().c_str());
			//get_handlers().process_event(event);
			//TODO: fill me
		}

		void clear_model()
		{
			//TODO: fill me
		}

		void walk(model::ConstReflectedObject& object, const model::PathId& id_path)
		try
		{
			const utils::keystring& obj_id = object.id().is_string() ? object.id().as_keystring() : "";
			const auto& obj_id_path = id_path.add(obj_id);
			process_object(object, obj_id_path);
			if (object.children().has_changes())
				for (auto child : object.children())
				{
					if (child.has_changes())
						walk(child, obj_id_path);
					else
						on_remove(obj_id_path);
				}
		}
		catch (const model::UnexpectedItemTypeException&)
		{
			logger::warning("graphic", "Unexpected item type");
		}

		void process_object(model::ConstReflectedObject& object, const model::PathId& obj_id_path)
		{
			(void)object;
			(void)obj_id_path;
			//TODO: fill me
		}
		void on_remove(const model::PathId& obj_id_path)
		{
			logger::debug("graphic", "Remove object '{}'", obj_id_path);
			//TODO: fill me
		}
	};
}//namespace graphic
}//namespace pisk

