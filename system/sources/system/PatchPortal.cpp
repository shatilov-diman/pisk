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


#include <pisk/infrastructure/Logger.h>
#include "PatchPortal.h"

#include <memory>
#include <mutex>
#include <set>

namespace pisk
{
namespace system
{
namespace impl
{
	class PatchGateQueue
	{
		PatchQueue queue;

	public:
		PatchPtr pop() threadsafe
		{
			PatchPtr patch;
			if (queue.pop(patch) == false)
				return {};
			return patch;
		}
		void push(const PatchPtr& patch)
		{
			queue.push(patch);
		}
	};

	class PatchGates
	{
		std::mutex mutex;
		std::deque<std::weak_ptr<PatchGateQueue>> gates;
	public:
		void link(std::weak_ptr<PatchGateQueue> gate)
		{
			std::unique_lock<std::mutex> guard(mutex);
			gates.push_back(std::move(gate));
		}
		void push(const PatchPtr& patch)
		{
			std::unique_lock<std::mutex> guard(mutex);
			for (const auto& gate : gates)
				if (auto g = gate.lock())
					g->push(patch);
		}
	};

	class PatchGate:
		public system::PatchGate
	{
		std::shared_ptr<PatchGateQueue> gate;
		std::shared_ptr<PatchGates> gates;
		virtual PatchPtr pop() threadsafe final override
		{
			return gate->pop();
		}

		virtual void push(const PatchPtr& patch) threadsafe final override
		{
			gates->push(patch);
		}
	public:
		PatchGate(const std::shared_ptr<PatchGateQueue>& gate, const std::shared_ptr<PatchGates>& gates):
			gate(gate),
			gates(gates)
		{
		}
		virtual ~PatchGate()
		{
			logger::debug("patch_portal", "gate destroied");
		}
	};

	class PatchPortal:
		public system::PatchPortal
	{
		std::mutex mutex;
		std::deque<std::weak_ptr<PatchGateQueue>> list_of_gate;
		std::deque<std::weak_ptr<PatchGates>> list_of_gates;

		virtual PatchGatePtr make_gate() threadsafe final override
		{
			logger::debug("patch_portal", "making new gate");
			std::unique_lock<std::mutex> guard(mutex);

			const std::shared_ptr<PatchGateQueue>& gate = std::make_shared<PatchGateQueue>();
			for (auto gates : list_of_gates)
				if (auto gs = gates.lock())
					gs->link(gate);

			const std::shared_ptr<PatchGates>& gates = std::make_shared<PatchGates>();
			for (auto gateptr : list_of_gate)
				if (auto g = gateptr.lock())
					gates->link(g);

			list_of_gate.push_back(gate);
			list_of_gates.push_back(gates);
			logger::debug("patch_portal", "gate was made");
			return std::make_unique<PatchGate>(gate, gates);
		}
	};
	PatchPortalPtr create_patch_portal()
	{
		return std::make_unique<pisk::system::impl::PatchPortal>();
	}
}
}
}

pisk::system::PatchPortalPtr CreatePatchPortal()
{
	return pisk::system::impl::create_patch_portal();
}

