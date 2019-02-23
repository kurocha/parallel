//
//  Queue.hpp
//  This file is part of the "Parallel" project and released under the .
//
//  Created by Samuel Williams on 23/2/2019.
//  Copyright, 2019, by Samuel Williams. All rights reserved.
//

#pragma once

#include <vector>
#include <mutex>

namespace Parallel
{
	// Stream data from multiple writers to a single reader.
	template <typename ItemT>
	class Queue
	{
	public:
		using Items = std::vector<ItemT>;
		
		Queue()
		{
			_waiting = &_buffers[0];
			_processing = &_buffers[1];
		}
		
		// This function is thread-safe and re-entrant.
		void enqueue(const ItemT & item)
		{
			std::lock_guard<std::mutex> lock(_lock);
			
			_waiting->push_back(item);
		}
		
		// This function is thread-safe and re-entrant.
		template <class ...Args>
		void emplace(Args && ...args)
		{
			std::lock_guard<std::mutex> lock(_lock);
			
			_waiting->emplace_back(std::forward<Args>(args)...);
		}
		
		void flush()
		{
			std::lock_guard<std::mutex> lock(_lock);
			
			_waiting->resize(0);
		}
		
		// This function is not re-entrant and must be called by the owner of the queue.
		// It is designed this way because this queue is most useful and efficient in this configuration.
		const std::vector<ItemT> & dequeue()
		{
			{
				std::lock_guard<std::mutex> lock(_lock);
				
				std::swap(_processing, _waiting);
				_waiting->clear();
			}
			
			return *_processing;
		}
		
	protected:
		std::mutex _lock;
		Items _buffers[2];
		
		std::vector<ItemT> * _waiting, * _processing;
	};
}
