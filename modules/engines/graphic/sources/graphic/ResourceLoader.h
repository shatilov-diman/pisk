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

#include "resources/resources.h"

#include "gl/Program.h"

#include <pisk/utils/keystring.h>

#include <pisk/tools/Releasable.h>

#include <map>

namespace pisk
{
namespace graphic
{
	class ResourceLoader :
		public tools::Releasable
	{
	public:
		using res_ids_t = std::map<utils::keystring, utils::keystring>;

		virtual ProgramPtr load_program(const res_ids_t& ids) = 0;
	};
	using ResourceLoaderPtr = tools::unique_relesable_ptr<ResourceLoader>;
}//namespace graphic
}//namespace pisk

