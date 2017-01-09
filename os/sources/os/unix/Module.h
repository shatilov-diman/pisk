// Project pisk
// Copyright (C) 2016-2017 Dmitry Shatilov
//
// This file is a part of the module os of the project pisk.
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
			infrastructure::Logger::get().debug("module", "Free library 0x%x", module);
			::dlclose(module);
			module = NULL;
		}
	public:

		static infrastructure::ModulePtr load(const std::string& prefix, const std::string& basename)
		{
			const std::string& path = make_name(prefix, basename);
			HMODULE module = ::dlopen(path.c_str(), RTLD_NOW);
			const char* err = dlerror();
			if (module == nullptr)
			{
				infrastructure::Logger::get().error("module", "Load library '%s' was failed: %s", path.c_str(), err);
				return nullptr;
			}
			infrastructure::Logger::get().debug("module", "Load library '%s': 0x%x", path.c_str(), module);
			return tools::make_shared_releasable_raw<Module>(new Module(module));
		}
		static std::string make_name(const std::string& prefix, const std::string& basename)
		{
			return prefix + "lib" + basename + ".so";
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

