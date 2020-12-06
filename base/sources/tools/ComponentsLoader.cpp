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


#include <pisk/defines.h>
#include <pisk/utils/json_utils.h>
#include <pisk/infrastructure/Logger.h>
#include <pisk/infrastructure/Module.h>
#include <pisk/infrastructure/Exception.h>
#include <pisk/tools/ComponentsLoader.h>

#include <json/json.h>
#include <json/reader.h>

#include <string>
#include <set>

namespace pisk
{
namespace tools
{
namespace components
{
	DescriptionsList Parser::parse(const infrastructure::DataStream& data) threadsafe noexcept
	try
	{
		utils::property list = load_components_list(data);
		if (list.is_none())
		{
			logger::critical("component_loader", "Unable to load components descriptor");
			return {};
		}

		return parse_components_list(list);
	}
	catch (const infrastructure::Exception&)
	{
		logger::critical("component_loader", "Exception occured while load components descriptor");
		return {};
	}

	utils::property Parser::load_components_list(const infrastructure::DataStream& data)
	{
		utils::property root = utils::json::parse_json_to_property(data);
		if (root.is_array() == false)
		{
			logger::critical("component_loader", "Unable to parse components descriptor");
			return utils::property();
		}
		return root;
	}

	std::vector<std::string> parse_dependencies(const utils::property& desc)
	{
		if (desc["dependencies"].is_none())
			return {};
		if (not desc["dependencies"].is_array())
		{
			logger::warning("component_loader", "Unexpected type of 'dependencies' node; skip it");
			return {};
		}

		std::vector<std::string> out;
		for(const auto& libname : desc["dependencies"].as_array())
		{
			if (libname.second.is_string())
				out.push_back(libname.second.as_string());
			else
				logger::warning("component_loader", "Unexpected type of 'dependency'; skip it");
		}
		return out;
	}

	DescriptionsList Parser::parse_components_list(const utils::property& list)
	{
		DescriptionsList out;

		std::set<std::string> components_name_set;
		for (const utils::property& desc : list)
		{
			if (desc.is_dictionary() == false)
			{
				logger::error("component_loader", "Incorrect record at the component descriptor");
				continue;
			}
			if (desc["type"].is_string() == false || desc["module"].is_string() == false
				|| desc["factory"].is_string() == false || desc["component"].is_string() == false)
			{
				logger::error("component_loader", "No expected record at the component descriptor");
				continue;
			}

			const std::string& type_name = desc["type"].as_string();
			const std::string& component_name = desc["component"].as_string();
			const std::string& module_name = desc["module"].as_string();
			const std::string& factory_name = desc["factory"].as_string();
			const utils::property& config = desc["config"];
			std::vector<std::string>&& dependecies = parse_dependencies(desc);

			if (components_name_set.find(component_name) != components_name_set.end())
			{
				logger::error("component_loader", "Unable to load two component with same name: '{}'", component_name);
				continue;
			}
			out.emplace_back(Description {type_name, component_name, module_name, factory_name, config, std::move(dependecies)});
			components_name_set.insert(component_name);
		}
		return out;
	}

	using ModuleLoader = std::function<infrastructure::ModulePtr (const std::string& name)>;
	using DependecyModulesList = std::vector<infrastructure::ModulePtr>;

	static bool load_dependencies(const Description& desc, ModuleLoader get_module, DependecyModulesList& dependencies)
	try
	{
		for (const auto& libname : desc.dependencies)
		{
			logger::debug("component_loader", "Check dependency '{}' for module '{}' for component '{}'", libname, desc.module, desc.name);
			infrastructure::ModulePtr module = get_module(libname);
			if (module == nullptr)
			{
				logger::error("component_loader", "Unable to load dependency '{}' for module '{}' for component '{}'", libname, desc.module, desc.name);
				return false;
			}
			dependencies.push_back(module);
		}
		return true;
	}
	catch (const infrastructure::Exception&)
	{
		logger::error("component_loader", "Unable to load dependency component '{}'", desc.name);
		return false;
	}

	static infrastructure::ModulePtr load_module(const Description& desc, ModuleLoader get_module, const std::string& module_name)
	try
	{
		logger::debug("component_loader", "Load module '{}' for component '{}'", desc.module, desc.name);
		return get_module(module_name);
	}
	catch (const infrastructure::Exception&)
	{
		return {};
	}

	void Loader::load(
		const DescriptionsList& descriptions,
		std::function<infrastructure::ModulePtr (const std::string& name)> get_module,
		std::function<SafeComponentPtr (ComponentFactoryFn factory, infrastructure::ModulePtr, const utils::property&)> load_component,
		std::function<void (const std::string& name, SafeComponentPtr component)> store_component
	) threadsafe
	{
		if (get_module == nullptr or load_component == nullptr or store_component == nullptr)
			throw infrastructure::NullPointerException();

		for (const auto& desc : descriptions)
		{
			std::vector<infrastructure::ModulePtr> dependencies;
			if (not load_dependencies(desc, get_module, dependencies))
				continue;

			infrastructure::ModulePtr module = load_module(desc, get_module, desc.module);
			if (module == nullptr)
				continue;

			try
			{
				auto factory_getter = module->get_procedure<ComponentFactoryGetter>(desc.factory);
				if (factory_getter == nullptr)
				{
					logger::warning("component_loader", "Unable to locate factory '{}' for component '{}'", desc.factory, desc.name);
					continue;
				}
				auto factory = static_cast<ComponentFactoryFn>(factory_getter());
				if (factory == nullptr)
				{
					logger::warning("component_loader", "Unable to get factory '{}' for component '{}'", desc.factory, desc.name);
					continue;
				}
				auto component = load_component(factory, module, desc.config);
				if (component == nullptr)
				{
					logger::warning("component_loader", "Unable to load component '{}'", desc.name);
					continue;
				}
				store_component(desc.name, component);
				logger::info("component_loader", "Component '{}' loaded", desc.name);
			}
			catch (const infrastructure::Exception&)
			{
				logger::error("component_loader", "Unable to load component '{}'", desc.name);
			}
		}
	}
}
}
}

