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
		using ExternalFunction = std::function <Results (Arguments&& arguments)>;

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

