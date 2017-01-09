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

				infrastructure::Logger::get().info("app", "Loading base components");
				components::Loader::load(system_components_desc, configurator.module_loader, component_loader, store_component);

				infrastructure::Logger::get().info("app", "Configure core components");
				configurator.configure_core_components(temp_sl);

				infrastructure::Logger::get().info("app", "Register os components");
				load_os_components(configurator.os_components, component_loader, store_component);

				infrastructure::Logger::get().info("app", "Loading component's list");
				const components::DescriptionsList& components_description = configurator.components_list_provider(temp_sl);

				infrastructure::Logger::get().info("app", "Loading components from the list");
				components::Loader::load(components_description, configurator.module_loader, component_loader, store_component);

				infrastructure::Logger::get().info("app", "Configure components");
				configurator.configure_components(temp_sl);

				infrastructure::Logger::get().info("app", "Looking for a MainLoop component");
				loop_component = temp_sl.get<MainLoop>(MainLoop::uid);
				if (loop_component == nullptr)
				{
					infrastructure::Logger::get().critical("app", "Unable to create MainLoop, app: 0x%x", this);
					throw infrastructure::NullPointerException();
				}

				temp_sl.clear();//Save object for dummies
			}

			~ComponentsManager()
			{
				infrastructure::Logger::get().info("app", "Releasing components");
				loop_component.reset();

				using SafeWPtr = infrastructure::ModuleHolderProxy<core::Component, std::weak_ptr<core::Component>>;

				infrastructure::Logger::get().info("app", "Search leak in components");
				std::deque<SafeWPtr> weaks;
				for (const auto& cmp : components)
					weaks.emplace_back(cmp.weak());
				components.clear();
				for (const auto& weak : weaks)
					if (const auto& cmp = weak.lock())
						infrastructure::Logger::get().warning("app", "core::Component leak detected, name: %s, app: 0x%x", typeid(*cmp).name(), this);
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
					infrastructure::Logger::get().info("app", "Load os component '%s'", pr.name.c_str());
					auto component = load_component(pr.factory, {}, {});
					if (component == nullptr)
					{
						infrastructure::Logger::get().warning("app", "Os component just didn't make; skip it");
						continue;
					}
					store_component(pr.name.get_content(), component);
				}
			}
		};

		class Application :
			public tools::Application
		{
			EventsContainer events;
			InterfacePtr<MainLoop> loop;

		public:
			virtual void release() {
				delete this;
			}

			virtual EventsContainer& get_events() threadsafe noexcept {
				return events;
			}

			//TODO: threadsafe
			virtual void stop() threadsafe noexcept
			try
			{
				infrastructure::Logger::get().debug("app", "Stop loop, app: 0x%x", this);
				if (loop != nullptr)
					loop->stop();
			}
			catch (const infrastructure::Exception&)
			{
				infrastructure::Logger::get().error("app", "Exception caught while stop, app: 0x%x", this);
			}

			//TODO: threadsafe
			virtual void run(const AppConfigurator& configurator)
			try
			{
				infrastructure::Logger::get().debug("app", "Run app: 0x%x", this);
				try_run(configurator);
				infrastructure::Logger::get().debug("app", "Stop app: 0x%x", this);
			}
			catch (const infrastructure::Exception&)
			{
				infrastructure::Logger::get().critical("app", "Exception caught while run, app: 0x%x", this);
				throw;
			}
			catch (...)
			{
				infrastructure::Logger::get().critical("app", "Unknown exception caught while run, app: 0x%x", this);
				throw;
			}

		private:
			void try_run(const AppConfigurator& configurator)
			{
				ComponentsManager components(configurator);
				try
				{

					get_events().app().AppStart.emit();
					loop = components.get_loop();

					infrastructure::Logger::get().debug("app", "Before start loop, app: 0x%x", this);
					loop->run();
					infrastructure::Logger::get().debug("app", "After stop loop, app: 0x%x", this);

					//we will be here after call stop()
					loop.reset();
					get_events().app().AppStop.emit();
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

