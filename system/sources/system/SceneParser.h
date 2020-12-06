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
#include <pisk/utils/noncopyable.h>
#include <pisk/utils/property_tree.h>

#include <memory>
#include <vector>

namespace pisk
{
namespace system
{
	class SceneParser : public utils::noncopyable
	{
	public:
		typedef std::vector<std::pair<utils::keystring, utils::property>> Objects;

		//TODO: extend method with callback interface
		static Objects parse(const utils::property& description) noexcept;
	};
}
}
