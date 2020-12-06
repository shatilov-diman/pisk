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


#include <pisk/infrastructure/Logger.h>
#include <pisk/infrastructure/Module.h>

#include <Windows.h>

namespace pisk
{
namespace os
{
namespace impl
{
	class Module : public infrastructure::Module
	{
		HMODULE module;

		explicit Module(HMODULE module) :
			module(module)
		{}
		virtual ~Module()
		{
			logger::info("module", "Free library {}", module);
			::FreeLibrary(module);
			module = NULL;
		}
	public:

		static infrastructure::ModulePtr load(const std::string& prefix, const std::string& basename, const std::string& suffix)
		{
			const std::string& path = make_name(prefix, basename, suffix);
			HMODULE module = ::LoadLibrary(path.c_str());
			logger::info("module", "Load library '{}': {}, err: {}", path, module, ::GetLastError());
			if (module == nullptr)
				return nullptr;
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
			return reinterpret_cast<Module::ProcedurePtr>(::GetProcAddress(module, name.c_str()));
		}
	};
}
}
}

EXPORT pisk::infrastructure::ModulePtr CreateModule(const std::string& basename)
{
	return pisk::os::impl::Module::load("./", basename, ".dll");
}

