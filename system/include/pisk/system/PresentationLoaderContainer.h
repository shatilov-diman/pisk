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
#include <pisk/core/Object.h>

#include "ResourceManager.h"

#include <memory>
#include <vector>

namespace pisk
{
namespace system
{
	class PresentationLoader : public utils::noncopyable
	{
	public:
		virtual core::PresentationPtr load(const core::ObjectPtr& object, const utils::property& description, const system::ResourceManager& resources_manager) const noexcept = 0;
	};
	typedef std::unique_ptr<PresentationLoader> PresentationLoaderPtr;

	class PresentationLoaderContainer : public utils::noncopyable
	{
		std::map<utils::keystring, PresentationLoaderPtr> presentationloader;
	public:
		void set(const utils::keystring& engine, PresentationLoaderPtr&& loader) {
			if (loader == nullptr)
				throw infrastructure::NullPointerException();
			presentationloader[engine].swap(loader);
			if (loader != nullptr)
				throw infrastructure::LogicErrorException();
		}
		void remove(const utils::keystring& engine) {
			presentationloader.erase(engine);
		}
		PresentationLoader& get(const utils::keystring& engine) const
		try {
			return *presentationloader.at(engine);
		} catch (const std::out_of_range&) {
			throw infrastructure::OutOfRangeException();
		}
	};

}
}
