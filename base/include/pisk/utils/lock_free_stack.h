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

#include <atomic>
#include <thread>

namespace pisk
{
namespace utils
{
	template <typename type>
	class lock_free_stack
	{
		using value_t = type;

		struct node_t
		{
			node_t* next;
			value_t value;
		};

		std::atomic<node_t*> root{nullptr};
		std::atomic<node_t*> free_nodes{nullptr};

	public:

		~lock_free_stack()
		{
			clear();
			shrink();
		}

		void clear()
		{
			value_t tmp;
			while (pop(tmp))
				;
		}

		void shrink()
		{
			release_free_nodes();
		}

		void push(const value_t& value)
		{
			push(value_t{value});
		}

		void push(value_t&& value)
		{
			node_t* new_root = allocate_node(std::move(value));
			push(new_root);
		}

		bool pop(value_t& value)
		{
			node_t* ptr = pop();
			if (ptr == nullptr)
				return false;

			value = std::move(ptr->value);
			deallocate_node(ptr);
			return true;
		}

	private:
		void push(node_t* ptr)
		{
			for (;;)
			{
				node_t* node = root.load();
				ptr->next = node;
				if (root.compare_exchange_weak(node, ptr))
					return;

				std::this_thread::yield();
			}
		}

		node_t* pop()
		{
			for (;;)
			{
				node_t* node = root.load();
				if (node == nullptr)
					return nullptr;

				if (root.compare_exchange_weak(node, node->next))
					return node;

				std::this_thread::yield();
			}
		}

	private:
		node_t* allocate_node(value_t&& value)
		{
			for (;;)
			{
				node_t* node = free_nodes.load();
				if (node == nullptr)
					return new node_t{nullptr, std::move(value)};

				if (free_nodes.compare_exchange_weak(node, node->next))
					return new (node) node_t{nullptr, std::move(value)};

				std::this_thread::yield();
			}
		}

		void deallocate_node(node_t* ptr)
		{
			for (;;)
			{
				node_t* node = free_nodes.load();
				ptr->next = node;
				if (free_nodes.compare_exchange_weak(node, ptr))
					return;

				std::this_thread::yield();
			}
		}

		void release_free_nodes()
		{
			for (;;)
			{
				node_t* node = free_nodes.load();
				if (node == nullptr)
					return;

				if (free_nodes.compare_exchange_weak(node, node->next))
				{
					delete node;
					continue;
				}

				std::this_thread::yield();
			}
		}
	};

}//namespace utils
}//namespace pisk

