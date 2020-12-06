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

#include "resources.h"
#include <pisk/system/ResourceManager.h>

namespace pisk
{
namespace graphic
{
	class ShaderSource :
		public system::SubResource
	{
	public:
		virtual const utils::keystring& get_type() const = 0;

		virtual const utils::keystring& get_source() const = 0;
	};
	using ShaderSourcePtr = system::subresource_ptr<ShaderSource>;

	class ShaderResource :
		public system::Resource
	{
	public:
		constexpr static const char* resource_type = "shader";

		virtual ShaderSourcePtr load(const utils::keystring& type) threadsafe const noexcept = 0;
	};
	using ShaderResourcePtr = system::resource_ptr<ShaderResource>;

}//namespace graphic
}//namespace pisk

