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
			infrastructure::Logger::get().info("module", "Free library 0x%x", module);
			::FreeLibrary(module);
			module = NULL;
		}
	public:

		static infrastructure::ModulePtr load(const std::string& prefix, const std::string& basename)
		{
			const std::string& path = make_name(prefix, basename);
			HMODULE module = ::LoadLibrary(path.c_str());
			infrastructure::Logger::get().info("module", "Load library '%s': 0x%x, err: 0x%x", path.c_str(), module, ::GetLastError());
			if (module == nullptr)
				return nullptr;
			return tools::make_shared_releasable_raw<Module>(new Module(module));
		}
		static std::string make_name(const std::string& prefix, const std::string& basename)
		{
			return prefix + basename + ".dll";
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
	return pisk::os::impl::Module::load("./", basename);
}

