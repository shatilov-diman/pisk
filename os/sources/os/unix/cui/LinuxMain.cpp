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


#include "../../common.h"
#include "../../ConsoleLogger.h"
#include "../../OsAppInstance.h"

#include <pisk/tools/MainLoop.h>
#include <pisk/os/SysEvent.h>

EXPORT pisk::infrastructure::ModulePtr CreateModule(const std::string& basename);

pisk::tools::SafeComponentPtr __cdecl common_make_os_app_instance_component(const pisk::tools::ServiceRegistry&, const pisk::tools::InstanceFactory& factory, const pisk::utils::property&);
pisk::tools::SafeComponentPtr __cdecl syseventdispatcher_factory(const pisk::tools::ServiceRegistry& temp_sl, const pisk::tools::InstanceFactory& factory, const pisk::utils::property&);
pisk::tools::SafeComponentPtr __cdecl main_loop_factory(const pisk::tools::ServiceRegistry& temp_sl, const pisk::tools::InstanceFactory& factory, const pisk::utils::property&);


extern "C"
void run_application()
{
	pisk::os::common_init_logger(std::make_unique<pisk::os::impl::ConsoleLogger>());

	const pisk::tools::OsComponentList components {
		{pisk::tools::OsAppInstance::uid, &common_make_os_app_instance_component},
		{pisk::os::OSSysEventDispatcher::uid, &syseventdispatcher_factory},
		{pisk::tools::MainLoop::uid, &main_loop_factory},
	};

	pisk::os::common_run_application({
		components,
		&pisk::os::common_configure_core_components,
		&pisk::os::common_load_components_list,
		&pisk::os::common_configure_components,
		&CreateModule,
	});
}

