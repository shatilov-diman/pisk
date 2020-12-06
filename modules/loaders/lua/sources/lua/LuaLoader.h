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

