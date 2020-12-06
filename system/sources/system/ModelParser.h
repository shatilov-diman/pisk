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
#include <map>

namespace pisk
{
namespace system
{
	class ModelParser : public utils::noncopyable
	{
	public:
		typedef std::map<utils::keystring, utils::property> Presentations;

		//TODO: extend method with callback interface
		static Presentations parse(const utils::property& description) noexcept;
	};
	typedef std::unique_ptr<ModelParser> ModelParserPtr;
}
}
