// Project pisk
// Copyright (C) 2016-2017 Dmitry Shatilov
//
// This file is a part of the module script of the project pisk.
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
			assert(resource_manager != nullptr);
		}
		bool execute(const utils::keystring& resource_id, const utils::keystring& function, const Arguments& arguments)
		try
		{
			infrastructure::Logger::get().spam("script", "Execute %s:%s(%s)", resource_id.c_str(), function.c_str(), to_string(arguments).c_str());
			auto found = scripts.find(resource_id);
			if (found == scripts.end())
			{
				if (not load(resource_id))
					return false;
				found = scripts.find(resource_id);
			}
			const auto& results = found->second->execute(function, arguments);
			infrastructure::Logger::get().spam("script", "Script executed with results: %s", to_string(results).c_str());
			return true;
		}
		catch(const ScriptException& ex)
		{
			infrastructure::Logger::get().warning("script", "Execute %s:%s failed with error: %s", resource_id.c_str(), function.c_str(), ex.error_msg.c_str());
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
			infrastructure::Logger::get().warning("script", "Register function %s:%s.%s failed with error: %s", resource_id.c_str(), nameprefix.c_str(), name.c_str(), ex.error_msg.c_str());
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
			infrastructure::Logger::get().warning("script", "Unregister function %s:%s.%s failed with error: %s", resource_id.c_str(), nameprefix.c_str(), name.c_str(), ex.error_msg.c_str());
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
			infrastructure::Logger::get().warning("script", "Resource '%s' found but not suuported", resource_id.c_str());
			return false;
		}
		catch (const system::ResourceNotFound&)
		{
			infrastructure::Logger::get().warning("script", "Script '%s' not found", resource_id.c_str());
			return false;
		}
		catch (const ScriptException& ex)
		{
			infrastructure::Logger::get().warning("script", "Unable to load script: %s", ex.error_msg.c_str());
			return false;
		}
	};
}
}


