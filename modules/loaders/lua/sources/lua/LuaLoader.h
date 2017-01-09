// Project pisk
// Copyright (C) 2016-2017 Dmitry Shatilov
//
// This file is a part of the module lua of the project pisk.
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

#include <pisk/system/ResourceLoader.h>

#include <pisk/script/Script.h>

namespace pisk
{
namespace script
{
namespace loaders
{
	using ScriptFactoryFn = std::function<ScriptPtr (const infrastructure::DataBuffer& data)>;

	class LuaScriptResource :
		public script::Resource
	{
		ScriptFactoryFn script_factory;
		infrastructure::DataBuffer data;

		virtual ScriptPtr make_instance() threadsafe const final override
		{
			return script_factory(data);
		}

	public:
		LuaScriptResource(ScriptFactoryFn _script_factory, infrastructure::DataStreamPtr&& stream):
			script_factory(_script_factory),
			data(stream->readall())
		{}
	};

	class LuaLoader :
		public system::ResourceLoader
	{
		ScriptFactoryFn script_factory;

		virtual void release() final override
		{
			delete this;
		}

		virtual bool can_load(const infrastructure::DataStreamPtr&) const noexcept threadsafe final override
		{
			return true;
		}

		virtual system::ResourcePtr load(infrastructure::DataStreamPtr&& stream) const noexcept threadsafe final override
		{
			if (stream == nullptr)
				return nullptr;
			return std::make_shared<LuaScriptResource>(script_factory, std::move(stream));
		}

	public:
		explicit LuaLoader(ScriptFactoryFn _script_factory) :
			script_factory(_script_factory)
		{}
	};
}
}
}

