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

#include <pisk/utils/property_tree.h>
#include <pisk/infrastructure/DataStream.h>

#include <pisk/system/Resource.h>

#include <vector>
#include <algorithm>

namespace pisk
{
namespace script
{
	using Arguments = std::vector<utils::property>;
	using Results = std::vector<utils::property>;

	struct ScriptException :
		public infrastructure::Exception
	{
		enum ErrorFamily
		{
			unknown,
			success,
			syntax_error,
			runtime_error,
			memory_error,
			internal_error,
		};
		const utils::keystring error_msg;
		const ErrorFamily error_family;

		ScriptException(const utils::keystring& error_msg, const ErrorFamily error_family):
			error_msg(error_msg),
			error_family(error_family)
		{}
	};

	class Script :
		public system::SubResource
	{
	public:
		using ExternalFunction = std::function <Results (const Arguments& arguments)>;

		virtual bool register_external_function(const utils::keystring& nameprefix, const utils::keystring& name, ExternalFunction function) = 0;

		virtual bool unregister_external_function(const utils::keystring& nameprefix, const utils::keystring& name)  = 0;

		virtual Results execute(const utils::keystring& function, const Arguments& arguments) = 0;
	};
	using ScriptPtr = system::subresource_ptr<Script>;

	class Resource :
		public system::Resource
	{
	public:
		constexpr static const char* resource_type = "script";

		virtual ScriptPtr make_instance() threadsafe const = 0;
	};
	using ResourcePtr = system::resource_ptr<Resource>;
}
}

