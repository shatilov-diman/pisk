// Project pisk
// Copyright (C) 2016-2017 Dmitry Shatilov
//
// This file is a part of the module system of the project pisk.
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


#pragma once

#include <pisk/utils/noncopyable.h>
#include <pisk/utils/property_tree.h>

#include <pisk/system/PatchPtr.h>

#include <chrono>
#include <memory>

namespace pisk
{
namespace system
{
	class PatchRecipient
	{
	public:
		virtual void push(const PatchPtr& patch) noexcept threadsafe = 0;
	};

	class EngineStrategy :
		public utils::noncopyable
	{
	public:
		struct Configure
		{
			std::chrono::milliseconds update_interval = std::chrono::milliseconds(25);
		};

		virtual ~EngineStrategy() {}

		virtual Configure on_init_app() = 0;

		virtual void on_deinit_app() = 0;

		virtual void patch_scene(const PatchPtr& patch) = 0;

		virtual void update() = 0;
	};

	class EngineStrategyBase :
		public EngineStrategy
	{
		PatchRecipient& patch_recipient;
	public:
		explicit EngineStrategyBase(PatchRecipient& patch_recipient) :
			patch_recipient(patch_recipient)
		{}

		void push_changes(const Patch& patch) noexcept threadsafe
		{
			push_changes(std::make_shared<Patch>(patch));
		}
		void push_changes(Patch&& patch) noexcept threadsafe
		{
			push_changes(std::make_shared<Patch>(std::move(patch)));
		}
		void push_changes(const PatchPtr& patch) noexcept threadsafe
		{
			patch_recipient.push(patch);
		}
	};

	using PatchRecipientPtr = std::unique_ptr<PatchRecipient>();
	using EngineStrategyPtr = std::unique_ptr<EngineStrategy>;
	using StrategyFactory = std::function<EngineStrategyPtr (PatchRecipient&)>;
}
}

