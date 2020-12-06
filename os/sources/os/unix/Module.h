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

#include <pisk/infrastructure/Logger.h>
#include <pisk/infrastructure/Module.h>

#include <dlfcn.h>

namespace pisk
{
namespace os
{
namespace impl
{
	class Module : public infrastructure::Module
	{
		using HMODULE = void*;
		HMODULE module;

		explicit Module(HMODULE module) :
			module(module)
		{}
		virtual ~Module()
		{
			logger::debug("module", "Free library {}", module);
			::dlclose(module);
			module = NULL;
		}
	public:

		static infrastructure::ModulePtr load(const std::string& prefix, const std::string& basename, const std::string& suffix)
		{
			const std::string& path = make_name(prefix, basename, suffix);
			HMODULE module = ::dlopen(path.c_str(), RTLD_NOW);
			const char* err = dlerror();
			if (module == nullptr)
			{
				logger::error("module", "Load library '{}' was failed: {}", path, err);
				return nullptr;
			}
			logger::debug("module", "Load library '{}': {}", path, module);
			return tools::make_shared_releasable_raw<Module>(new Module(module));
		}
		static std::string make_name(const std::string& prefix, const std::string& basename, const std::string& suffix)
		{
			return prefix + basename + suffix;
		}

		virtual void release() final override
		{
			delete this;
		}
		virtual Module::ProcedurePtr find_procedure(const std::string& name) final override
		{
			void (*out)();
			*reinterpret_cast<void **>(&out) = ::dlsym(module, name.c_str());
			return static_cast<Module::ProcedurePtr>(out);
		}
	};
}
}
}

