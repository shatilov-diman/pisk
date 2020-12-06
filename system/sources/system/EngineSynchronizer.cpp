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
#include <pisk/infrastructure/Exception.h>

#include <pisk/utils/sync_flag.h>

#include "EngineSynchronizer.h"

namespace pisk
{
namespace system
{
namespace impl
{
	class Counter
	{
		mutable std::mutex guard;
		mutable std::condition_variable signaler;

		std::atomic_uint counter = { 0 };

	public:

		void increase()
		{
			std::unique_lock<std::mutex> lock(guard);
			counter++;
			signaler.notify_all();
			logger::debug("synchronizer", "Synchronizer {}, signaled slaves: {}", this, counter.load());
		}

		void wait_at_least(const unsigned int limit) const
		{
			while (true)
			{
				std::unique_lock<std::mutex> lock(guard);
				if (counter >= limit)
					break;
				signaler.wait(lock);
			}
		}
	};

	class EngineSynchronizer :
		public tools::enable_shared_from_this<EngineSynchronizer>,
		public system::EngineSynchronizer
	{
		std::atomic_uint clients_count;
		std::atomic_bool increase_clients_locked;
		std::atomic_bool stop_requested;

		Counter ready_counter;
		Counter initialized_counter;
		Counter loop_finished_counter;
		Counter deinitialized_counter;

		utils::sync::flag sync_initialize;
		utils::sync::flag sync_run_loop;
		utils::sync::flag sync_deinitialize;

	public:
		EngineSynchronizer()
		{
			logger::debug("synchronizer", "Synchronizer {} is constructing", this);

			stop_requested = false;

			clients_count = 0;
			increase_clients_locked = false;
		}

		virtual void release() override
		{
			logger::debug("synchronizer", "Synchronizer {} is destroing", this);
			delete this;
		}

		virtual EngineSynchronizerSlavePtr make_slave() final override;

		virtual void wait_all_ready() final override
		{
			logger::debug("synchronizer", "Synchronizer {} is waiting slaves to be ready", this);
			increase_clients_locked = true;
			ready_counter.wait_at_least(clients_count);
		}

		virtual void initialize_signal() final override
		{
			logger::debug("synchronizer", "Synchronizer {} raise inititalize signal", this);
			sync_initialize.set();
		}

		virtual void wait_all_initialized() final override
		{
			logger::debug("synchronizer", "Synchronizer {} is waiting slaves to be initialized", this);
			initialized_counter.wait_at_least(clients_count);
		}

		virtual void run_loop_signal() final override
		{
			logger::debug("synchronizer", "Synchronizer {} raise loop begin signal", this);
			sync_run_loop.set();
		}

		virtual void stop_all() final override
		{
			logger::debug("synchronizer", "Synchronizer {} stop requested", this);
			stop_requested = true;
		}

		virtual void wait_all_loop_finished() final override
		{
			logger::debug("synchronizer", "Synchronizer {} is waiting slaves to be finished", this);
			loop_finished_counter.wait_at_least(clients_count);
		}

		virtual void deinitialize_signal() final override
		{
			logger::debug("synchronizer", "Synchronizer {} raise deinititalize signal", this);
			sync_deinitialize.set();
		}

		virtual void wait_all_deinitialized() final override
		{
			logger::debug("synchronizer", "Synchronizer {} is waiting slaves to be deinitialized", this);
			deinitialized_counter.wait_at_least(clients_count);
		}

	public:
		void increase_client()
		{
			if (increase_clients_locked)
				throw infrastructure::LogicErrorException();
			clients_count++;
			logger::debug("synchronizer", "Synchronizer {}, increase slaves: {}", this, clients_count.load());
		}

		void decrease_client()
		{
			clients_count--;
			logger::debug("synchronizer", "Synchronizer {}, decrease slaves: {}", this, clients_count.load());
		}

		bool is_stop_requested() const
		{
			return stop_requested;
		}

		void notify_ready()
		{
			ready_counter.increase();
		}

		void wait_initialize_signal()
		{
			sync_initialize.wait();
		}
		void notify_initialize_finished()
		{
			initialized_counter.increase();
		}

		void wait_loop_begin_signal()
		{
			sync_run_loop.wait();
		}
		void notify_loop_finished()
		{
			loop_finished_counter.increase();
		}

		void wait_deinitialize_signal()
		{
			sync_deinitialize.wait();
		}
		void notify_deinitialize_finished()
		{
			deinitialized_counter.increase();
		}
	};
	using EngineSynchronizerPtr = tools::shared_relesable_ptr<EngineSynchronizer>;

	class EngineSynchronizerSlave :
		public system::EngineSynchronizerSlave
	{
		EngineSynchronizerPtr syncronizer;

	public:
		explicit EngineSynchronizerSlave(const EngineSynchronizerPtr& _syncronizer):
			syncronizer(_syncronizer)
		{
			logger::debug("synchronizer", "Slave {} is constructing", this);
			syncronizer->increase_client();
		}

		virtual ~EngineSynchronizerSlave()
		{
			logger::debug("synchronizer", "Slave {} is destroing", this);
			syncronizer->decrease_client();
		}

		virtual void release() final override
		{
			delete this;
		}

		virtual bool is_stop_requested() final override
		{
			return syncronizer->is_stop_requested();
		}

		virtual void notify_ready() final override
		{
			logger::debug("synchronizer", "Slave {} ready", this);
			return syncronizer->notify_ready();
		}

		virtual void wait_initialize_signal() final override
		{
			logger::debug("synchronizer", "Slave {} is waiting initialize signal", this);
			syncronizer->wait_initialize_signal();
			logger::debug("synchronizer", "Slave {} has received initialize signal", this);
		}
		virtual void notify_initialize_finished() final override
		{
			logger::debug("synchronizer", "Slave {} has initialized", this);
			syncronizer->notify_initialize_finished();
		}

		virtual void wait_loop_begin_signal() final override
		{
			logger::debug("synchronizer", "Slave {} is waiting loop begin signal", this);
			syncronizer->wait_loop_begin_signal();
			logger::debug("synchronizer", "Slave {} has received loop begin signal", this);
		}
		virtual void notify_loop_finished() final override
		{
			logger::debug("synchronizer", "Slave {} has finished", this);
			syncronizer->notify_loop_finished();
		}

		virtual void wait_deinitialize_signal() final override
		{
			logger::debug("synchronizer", "Slave {} is waiting deinitialize signal", this);
			syncronizer->wait_deinitialize_signal();
			logger::debug("synchronizer", "Slave {} has received deinitialize signal", this);
		}
		virtual void notify_deinitialize_finished() final override
		{
			logger::debug("synchronizer", "Slave {} has deinitialized", this);
			syncronizer->notify_deinitialize_finished();
		}
	};

	EngineSynchronizerSlavePtr EngineSynchronizer::make_slave()
	{
		return tools::make_unique_releasable<EngineSynchronizerSlave>(shared_from_this());
	}

}//namespace impl

	EngineSynchronizerPtr make_engine_synchronizer()
	{
		return tools::make_shared_releasable<pisk::system::impl::EngineSynchronizer>();
	}

}//namespace system
}//namespace pisk

