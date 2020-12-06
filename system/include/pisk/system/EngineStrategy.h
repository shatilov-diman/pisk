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

		virtual void prepatch() {}

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

		void push_changes(const Patch& patch) const noexcept threadsafe
		{
			push_changes(std::make_shared<Patch>(patch));
		}
		void push_changes(Patch&& patch) const noexcept threadsafe
		{
			push_changes(std::make_shared<Patch>(std::move(patch)));
		}
		void push_changes(const PatchPtr& patch) const noexcept threadsafe
		{
			patch_recipient.push(patch);
		}
	};

	using PatchRecipientPtr = std::unique_ptr<PatchRecipient>();
	using EngineStrategyPtr = std::unique_ptr<EngineStrategy>;
	using StrategyFactory = std::function<EngineStrategyPtr (PatchRecipient&)>;
}
}

