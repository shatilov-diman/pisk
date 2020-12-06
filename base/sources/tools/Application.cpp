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


#include <pisk/utils/algorithm_utils.h>

#include <pisk/infrastructure/Logger.h>
#include <pisk/tools/MainLoop.h>
#include <pisk/tools/Application.h>
#include <pisk/tools/InstanceFactory.h>
#include <pisk/tools/ServiceRegistry.h>

#include "ServiceLocator.h"

#include <assert.h>

namespace pisk
{
namespace tools
{
	namespace impl
	{
		class ComponentInstanceFactory :
			public tools::InstanceFactory
		{
			infrastructure::ModulePtr module;

			virtual SafeComponentPtr safe_instance(const std::shared_ptr<core::Component>& instance) const final override
			{
				return {module, instance};
			}
		public:
			explicit ComponentInstanceFactory(const infrastructure::ModulePtr& module) :
				module(module)
			{}
		};

		class ServiceRegistry :
			public tools::ServiceRegistry
		{
			ServiceLocator<SafeComponentPtr> sl;

			virtual SafeComponentPtr find(const UID& uid) const final override
			{
				return sl.get(uid);
			}
		public:
			void clear()
			{
				sl.clear();
			}
			void replace(const UID& key, const SafeComponentPtr& component)
			{
				sl.replace(key, component);
			}
		};


		class ComponentsManager
		{
			const components::DescriptionsList system_components_desc = components::DescriptionsList {
//				components::Description {"service", "logger", "", "logger_factory"},
				components::Description {"service", "resource_manager", "system", "get_resource_manager_factory"},
				components::Description {"resource_loader", "property_tree_loader", "system", "get_property_tree_loader_factory"},
				components::Description {"service", "engine_component_factory", "system", "get_engine_component_factory_factory"},
				components::Description {"service", "events_manager", "system", "get_events_manager_factory"},
			};

			ServiceRegistry temp_sl;

			std::vector<SafeComponentPtr> components;
			InterfacePtr<MainLoop> loop_component;
		public:
			explicit ComponentsManager(const AppConfigurator& configurator)
			{
				if (configurator.components_list_provider == nullptr
				    or configurator.configure_components == nullptr
				    or configurator.configure_core_components == nullptr
				    or configurator.module_loader == nullptr)
				{
					throw infrastructure::NullPointerException();
				}

				auto component_loader = [this](components::ComponentFactoryFn factory, infrastructure::ModulePtr module, const utils::property& config) -> SafeComponentPtr {
					ComponentInstanceFactory instance_maker(module);
					return factory(temp_sl, instance_maker, config);
				};
				auto store_component = [this](const std::string& name, SafeComponentPtr safecomponent) {
					temp_sl.replace(utils::keystring(name), safecomponent);
					components.push_back(safecomponent);
				};

				logger::info("app", "Register os components");
				load_os_components(configurator.os_components, component_loader, store_component);

				logger::info("app", "Loading base components");
				components::Loader::load(system_components_desc, configurator.module_loader, component_loader, store_component);

				logger::info("app", "Configure core components");
				configurator.configure_core_components(temp_sl);

				logger::info("app", "Loading component's list");
				const components::DescriptionsList& components_description = configurator.components_list_provider(temp_sl);

				logger::info("app", "Loading components from the list");
				components::Loader::load(components_description, configurator.module_loader, component_loader, store_component);

				logger::info("app", "Configure components");
				configurator.configure_components(temp_sl);

				logger::info("app", "Looking for a MainLoop component");
				loop_component = temp_sl.get<MainLoop>(MainLoop::uid);
				if (loop_component == nullptr)
				{
					logger::critical("app", "Unable to create MainLoop, app: {}", this);
					throw infrastructure::NullPointerException();
				}

				temp_sl.clear();//Save object for dummies
			}

			~ComponentsManager()
			{
				logger::info("app", "Releasing components");
				loop_component.reset();

				using SafeWPtr = infrastructure::ModuleHolderProxy<core::Component, std::weak_ptr<core::Component>>;

				logger::info("app", "Search leak in components");
				std::deque<SafeWPtr> weaks;
				for (const auto& cmp : components)
					weaks.emplace_back(cmp.weak());
				components.clear();
				for (const auto& weak : weaks)
					if (const auto& cmp = weak.lock())
						logger::warning("app", "core::Component leak detected, name: {}, app: {}", typeid(decltype((*cmp))).name(), this);
				while (not weaks.empty())//Insure that modules releasing in backward order
					weaks.pop_back();
			}
			InterfacePtr<MainLoop> get_loop()
			{
				return loop_component;
			}
		private:
			static void load_os_components(
				const OsComponentList& os_components,
				std::function<SafeComponentPtr (components::ComponentFactoryFn factory, infrastructure::ModulePtr, const utils::property&)> load_component,
				std::function<void (const std::string& name, SafeComponentPtr component)> store_component
			)
			{
				for (auto& pr : os_components)
				{
					logger::info("app", "Load os component '{}'", pr.name);
					auto component = load_component(pr.factory, {}, {});
					if (component == nullptr)
					{
						logger::warning("app", "Os component just didn't make; skip it");
						continue;
					}
					store_component(pr.name.get_content(), component);
				}
			}
		};

		class Application :
			public tools::Application
		{
			InterfacePtr<MainLoop> loop;

		public:
			virtual void release() {
				delete this;
			}

			//TODO: threadsafe
			virtual void stop() threadsafe noexcept
			try
			{
				logger::debug("app", "Stop loop, app: {}", this);
				if (loop != nullptr)
					loop->stop();
			}
			catch (const infrastructure::Exception&)
			{
				logger::error("app", "Exception caught while stop, app: {}", this);
			}

			//TODO: threadsafe
			virtual void run(const AppConfigurator& configurator)
			try
			{
				logger::debug("app", "Run app: {}", this);
				try_run(configurator);
				logger::debug("app", "Stop app: {}", this);
			}
			catch (const infrastructure::Exception&)
			{
				logger::critical("app", "Exception caught while run, app: {}", this);
				throw;
			}
			catch (...)
			{
				logger::critical("app", "Unknown exception caught while run, app: {}", this);
				throw;
			}

		private:
			void try_run(const AppConfigurator& configurator)
			{
				ComponentsManager components(configurator);
				try
				{
					loop = components.get_loop();

					logger::debug("app", "Before start loop, app: {}", this);
					loop->spinup();
					logger::debug("app", "After stop loop, app: {}", this);

					//we will be here after call stop()
					loop.reset();
				}
				catch (...)
				{
					loop.reset();
					throw;
				}
			}
		};
	}

	ApplicationPtr EXPORT make_application()
	{
		return make_unique_releasable<impl::Application>();
	}
}
}

