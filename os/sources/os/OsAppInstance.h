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
#include <pisk/tools/OsAppInstance.h>

namespace pisk
{
namespace os
{
namespace impl
{

class OsAppInstance:
       public tools::OsAppInstance
{
       virtual void release() noexcept final override
       {
               delete this;
       }
};

}
}
}


