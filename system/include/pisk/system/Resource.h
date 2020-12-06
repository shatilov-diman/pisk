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

#include <memory>

namespace pisk
{
namespace system
{
	/* It's safe to use resource without hold loader module.
	 *
	 * Loader modules are held by ResourceManager(system module).
	 * Only engines use resources. All engines modules
	 * loaded after ResourceManager and freed before
	 * ResourceManager(by design). So all consumers of
	 * resources are released before ResourceManager start
	 * to release loader modules.
	 */
	template <typename Type>
	using resource_ptr = std::shared_ptr<Type>;

	class Resource :
		public utils::noncopyable
	{
	public:
		virtual ~Resource() {}
	};
	using ResourcePtr = resource_ptr<Resource>;


	template <typename Type>
	using subresource_ptr = std::unique_ptr<Type>;

	class SubResource:
		public utils::noncopyable
	{
	public:
		virtual ~SubResource() {}
	};
	using SubResourcePtr = subresource_ptr<SubResource>;
}
}

