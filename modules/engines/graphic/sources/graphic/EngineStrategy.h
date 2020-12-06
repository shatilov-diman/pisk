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

#include <pisk/utils/json_utils.h>

#include <pisk/system/EngineStrategy.h>

#include "EngineController.h"
#include "ResourceLoader.h"

namespace pisk
{
namespace graphic
{
	class EngineStrategy :
		public system::EngineStrategyBase
	{
		EngineController engine_controller;

	public:
		EngineStrategy(
			const pisk::utils::property& config,
			const os::WindowManagerPtr& window_manager,
			system::PatchRecipient& patch_recipient,
			ResourceLoaderPtr&& resource_loader
		):
			system::EngineStrategyBase(patch_recipient),
			engine_controller(config, window_manager, std::move(resource_loader))
		{
			engine_controller.on_activate_singaler += [this]() {
				this->push_activate_event(true);
			};
			engine_controller.on_deactivate_singaler += [this]() {
				this->push_activate_event(false);
			};
			engine_controller.on_close_request_signaler += [this]() {
				this->push_close_request_event();
			};
			engine_controller.on_close_signaler += [this]() {
				this->push_close_event();
			};
		}

	private:
		virtual Configure on_init_app() final override
		{
			engine_controller.on_init_app();
			return {};
		}

		virtual void on_deinit_app() final override
		{
			engine_controller.on_deinit_app();
		}

		virtual void prepatch() final override
		{
			engine_controller.prepatch();
		}

		virtual void patch_scene(const pisk::system::PatchPtr& scene) final override
		{
			engine_controller.apply_changes(scene);
		}

		virtual void update() final override
		{
			engine_controller.update();
		}

	private:
		void push_activate_event(const bool activate)
		{
			system::Patch patch;
			model::ReflectedEvent event(utils::property::none_property(), patch);
			event.source() = "graphic";
			event.type() = "window";
			event.action() = activate ? "activate" : "deactivate";

			system::Patch out;
			out["events"][std::size_t(0)] = patch;
			push(std::move(out));
		}
		void push_close_request_event()
		{
			system::Patch patch;
			model::ReflectedEvent event(utils::property::none_property(), patch);
			event.source() = "graphic";
			event.type() = "window";
			event.action() = "close_request";

			system::Patch out;
			out["events"][std::size_t(0)] = patch;
			push(std::move(out));
		}
		void push_close_event()
		{
			system::Patch patch;
			model::ReflectedEvent event(utils::property::none_property(), patch);
			event.source() = "graphic";
			event.type() = "window";
			event.action() = "close";

			system::Patch out;
			out["events"][std::size_t(0)] = patch;
			push(std::move(out));
		}
		void push(pisk::system::Patch&& patch) noexcept threadsafe
		{
			push(std::make_shared<pisk::system::Patch>(std::move(patch)));
		}
		void push(const pisk::system::PatchPtr& patch) noexcept threadsafe
		{
			const auto& content = pisk::utils::json::to_string(*patch);
			pisk::logger::debug("graphic", "Update scene:\n {}", content.c_str());

			push_changes(patch);
		}
	};
}//namespace graphic
}//namespace pisk

