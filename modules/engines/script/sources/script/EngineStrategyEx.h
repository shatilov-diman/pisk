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
#include <pisk/utils/algorithm_utils.h>
#include <pisk/utils/json_utils.h>
#include <pisk/infrastructure/Logger.h>

#include <pisk/script/Script.h>


namespace pisk
{
namespace script
{
	constexpr const char pisk_namprefix[] = "pisk";

	template <typename BaseStrategy>
	class PushChangesEngineStrategy :
		public BaseStrategy
	{
		const utils::keystring push_changes_member {"push_changes"};

	public:
		template <typename ... TArgs>
		PushChangesEngineStrategy(TArgs& ... args):
			BaseStrategy(args...)
		{}

	protected:
		using Configure = typename BaseStrategy::Configure;

		virtual Configure on_init_app() override
		{
			if (not this->get_script_manager().register_external_function(
				"script", pisk_namprefix, push_changes_member,
				[this] (auto&& args) { return this->push_changes_wrapper(std::move(args)); }))
			{
				logger::warning("script", "Unalbe to register '{}:{}' external function", pisk_namprefix, push_changes_member);
			}

			return BaseStrategy::on_init_app();
		}

		virtual void on_deinit_app() override
		{
			BaseStrategy::on_deinit_app();
			if (not this->get_script_manager().unregister_external_function("script", pisk_namprefix, push_changes_member))
				logger::warning("script", "Unalbe to unregister '{}:{}' external function", pisk_namprefix, push_changes_member);
		}

		Results push_changes_wrapper(Arguments&& arguments)
		{
			if (arguments.size() != 1)
			{
				logger::warning("script", "Unexpected argumens count while call 'push_changes'");
				throw infrastructure::InvalidArgumentException();
			}
			if (not arguments[0].is_dictionary())
			{
				logger::warning("script", "'push_changes' required only table with string as key");
				throw infrastructure::InvalidArgumentException();
			}

			this->push_changes(std::move(arguments[0]));

			return {};
		}
	};

	template <typename BaseStrategy>
	class StopAppEngineStrategy :
		public BaseStrategy
	{
		const utils::keystring stop_app_member {"stop_app"};

		tools::MainLoopPtr main_loop;

	public:
		template <typename ... TArgs>
		StopAppEngineStrategy(const tools::MainLoopPtr& main_loop, TArgs& ... args):
			BaseStrategy(args...),
			main_loop(main_loop)
		{}

	protected:
		using Configure = typename BaseStrategy::Configure;

		virtual Configure on_init_app() override
		{
			if (not this->get_script_manager().register_external_function(
				"script", pisk_namprefix, stop_app_member,
				[this] (auto&& args) { return this->stop_app_wrapper(std::move(args)); }))
			{
				logger::warning("script", "Unalbe to register '{}:{}' external function", pisk_namprefix, stop_app_member);
			}

			return BaseStrategy::on_init_app();
		}

		virtual void on_deinit_app() override
		{
			BaseStrategy::on_deinit_app();
			if (not this->get_script_manager().unregister_external_function("script", pisk_namprefix, stop_app_member))
				logger::warning("script", "Unalbe to unregister '{}:{}' external function", pisk_namprefix, stop_app_member);
		}

	private:
		Results stop_app_wrapper(Arguments&& arguments)
		{
			if (arguments.size() != 0)
			{
				logger::warning("script", "Unexpected argumens count while call 'stop_app'");
				throw infrastructure::InvalidArgumentException();
			}

			this->stop_app();

			return {};
		}

		void stop_app()
		{
			logger::info("script", "Stop application request has been received");
			main_loop->stop();
		}
	};


	template <typename BaseStrategy>
	class LogEngineStrategy :
		public BaseStrategy
	{
		const utils::keystring log_member {"log"};

	public:
		template <typename ... TArgs>
		LogEngineStrategy(TArgs& ... args):
			BaseStrategy(args...)
		{}

	protected:
		using Configure = typename BaseStrategy::Configure;

		virtual Configure on_init_app() override
		{
			if (not this->get_script_manager().register_external_function(
				"script", pisk_namprefix, log_member,
				[this] (auto&& args) { return this->log(std::move(args)); }))
			{
				logger::warning("script", "Unalbe to register '{}:{}' external function", pisk_namprefix, log_member);
			}

			return BaseStrategy::on_init_app();
		}

		virtual void on_deinit_app() override
		{
			BaseStrategy::on_deinit_app();
			if (not this->get_script_manager().unregister_external_function("script", pisk_namprefix, log_member))
				logger::warning("script", "Unalbe to unregister '{}:{}' external function", pisk_namprefix, log_member);
		}

		Results log(Arguments&& arguments)
		{
			if (arguments.size() != 2)
			{
				logger::error("script", "Unexpected count of arguments for 'log' external function");
				return {};
			}
			if (not arguments[0].is_string())
			{
				logger::error("script", "Unexpected arguments for 'log' external function");
				return {};
			}

			const infrastructure::Logger::Level loglevel = arg_to_loglevel(arguments[0].as_keystring());
			logger::log(loglevel, "script log", utils::json::to_string(arguments[1]));
			return {};
		}
		static infrastructure::Logger::Level arg_to_loglevel(const utils::keystring& loglevel)
		{
			if (loglevel == "spam")
				return infrastructure::Logger::Level::Spam;
			if (loglevel == "debug")
				return infrastructure::Logger::Level::Debug;
			if (loglevel == "info")
				return infrastructure::Logger::Level::Information;
			if (loglevel == "warning")
				return infrastructure::Logger::Level::Warning;
			if (loglevel == "error")
				return infrastructure::Logger::Level::Error;
			return infrastructure::Logger::Level::Information;
		}
	};

	template <typename BaseStrategy>
	class LoadSceneEngineStrategy :
		public BaseStrategy
	{
		const utils::keystring load_scene_member {"load_scene"};

		system::ResourceManagerPtr resource_manager;

	public:
		template <typename ... TArgs>
		LoadSceneEngineStrategy(const system::ResourceManagerPtr& _resource_manager, TArgs& ... args):
			BaseStrategy(_resource_manager, args...),
			resource_manager(_resource_manager)
		{
			if (resource_manager == nullptr)
				throw infrastructure::InvalidArgumentException();
		}

	protected:
		using Configure = typename BaseStrategy::Configure;

		virtual Configure on_init_app() override
		{
			if (not this->get_script_manager().register_external_function(
				"script", pisk_namprefix, load_scene_member,
				[this](auto&& args) { return this->script_load_scene(std::move(args)); }
			))
			{
				logger::warning("script", "Unalbe to register '{}:{}' external function", pisk_namprefix, load_scene_member);
			}

			return BaseStrategy::on_init_app();
		}

		virtual void on_deinit_app() override
		{
			BaseStrategy::on_deinit_app();
			if (not this->get_script_manager().unregister_external_function("script", pisk_namprefix, load_scene_member))
				logger::warning("script", "Unalbe to unregister '{}:{}' external function", pisk_namprefix, load_scene_member);
		}

		Results script_load_scene(Arguments&& arguments)
		{
			static const char signature[] = "load_scene(path_to_scene:string, clear_all:bool)";
			if (arguments.size() != 2)
			{
				logger::error("script", "Unexpected count of arguments for external function '{}'", signature);
				return {};
			}
			if (not arguments[0].is_string())
			{
				logger::error("script", "Unexpected 1st argument for external function '{}'", signature);
				return {};
			}
			if (not arguments[1].is_bool())
			{
				logger::error("script", "Unexpected 2nd argument for external function '{}'", signature);
				return {};
			}

			this->load_scene(arguments[0].as_keystring(), arguments[1].as_bool());

			return {};
		}
	};
}// namespace script
}// namespace pisk

