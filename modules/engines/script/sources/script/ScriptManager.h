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
#include <pisk/infrastructure/Logger.h>
#include <pisk/utils/keystring.h>
#include <pisk/utils/property_tree.h>
#include <pisk/utils/json_utils.h>

#include <pisk/system/ResourceManager.h>

#include <pisk/script/Script.h>

#include <map>
#include <iterator>
#include <algorithm>


namespace pisk
{
namespace script
{
	class ScriptManager
	{
		using Scripts = std::map<utils::keystring, ScriptPtr>;

		system::ResourceManagerPtr resource_manager;
		Scripts scripts;

	public:
		explicit ScriptManager(const system::ResourceManagerPtr& _resource_manager) :
			resource_manager(_resource_manager)
		{
			if (resource_manager == nullptr)
			{
				logger::error("script", "Unable to locate resource manager");
				throw infrastructure::InvalidArgumentException();
			}
		}
		bool execute(const utils::keystring& resource_id, const utils::keystring& function, const Arguments& arguments)
		try
		{
			logger::spam("script", "Execute {}:{}({})", resource_id.c_str(), function, to_string(arguments));
			auto found = scripts.find(resource_id);
			if (found == scripts.end())
			{
				if (not load(resource_id))
					return false;
				found = scripts.find(resource_id);
			}
			const auto& results = found->second->execute(function, arguments);
			logger::spam("script", "Script executed with results: {}", to_string(results));
			return true;
		}
		catch(const ScriptException& ex)
		{
			logger::warning("script", "Execute {}:{} failed with error: {}", resource_id, function, ex.error_msg);
			return false;
		}

		bool register_external_function(const utils::keystring& resource_id, const utils::keystring& nameprefix, const utils::keystring& name, Script::ExternalFunction function)
		try
		{
			auto found = scripts.find(resource_id);
			if (found == scripts.end())
			{
				if (not load(resource_id))
					return false;
				found = scripts.find(resource_id);
			}
			return found->second->register_external_function(nameprefix, name, function);
		}
		catch(const ScriptException& ex)
		{
			logger::warning("script", "Register function {}:{}.{} failed with error: {}", resource_id.c_str(), nameprefix, name, ex.error_msg);
			return false;
		}

		bool unregister_external_function(const utils::keystring& resource_id, const utils::keystring& nameprefix, const utils::keystring& name)
		try
		{
			auto found = scripts.find(resource_id);
			if (found == scripts.end())
				return false;
			return found->second->unregister_external_function(nameprefix, name);
		}
		catch(const ScriptException& ex)
		{
			logger::warning("script", "Unregister function {}:{}.{} failed with error: {}", resource_id.c_str(), nameprefix, name, ex.error_msg);
			return false;
		}

	private:
		static std::string to_string(const Arguments& args)
		{
			std::vector<std::string> arg_strings;
			std::transform(
				args.begin(),
				args.end(),
				std::back_inserter(arg_strings),
				std::bind(&utils::json::to_string, std::placeholders::_1)
			);
			return '[' + utils::algorithm::join(arg_strings, std::string(",")) + ']';
		}

		bool load(const utils::keystring& resource_id)
		try
		{
			const auto& resource = resource_manager->load<script::Resource>(resource_id.get_content());
			ScriptPtr&& script = resource->make_instance();
			scripts[resource_id] = std::move(script);
			return true;
		}
		catch (const system::ResourceFormatNotSupported&)
		{
			logger::warning("script", "Resource '{}' found but not supported", resource_id);
			return false;
		}
		catch (const system::ResourceNotFound&)
		{
			logger::warning("script", "Script '{}' not found", resource_id);
			return false;
		}
		catch (const ScriptException& ex)
		{
			logger::warning("script", "Unable to load script: {}", ex.error_msg);
			return false;
		}
	};
}
}


