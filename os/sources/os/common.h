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
#include <pisk/infrastructure/Logger.h>
#include <pisk/tools/Application.h>

namespace pisk
{
namespace os
{

void common_init_logger(std::unique_ptr<infrastructure::LogStorage>&& log_storage);

void common_configure_core_components(const tools::ServiceRegistry& temp_sl);

tools::components::DescriptionsList common_load_components_list(const tools::ServiceRegistry& temp_sl);

void common_configure_components(const tools::ServiceRegistry& temp_sl);

void common_run_application(const tools::AppConfigurator& configurator);

}
}

