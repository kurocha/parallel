//
//  Distributor.hpp
//  File file is part of the "Concurrent" project and released under the MIT License.
//
//  Created by Samuel Williams on 29/6/2017.
//  Copyright, 2017, by Samuel Williams. All rights reserved.
//

#pragma once

#include <condition_variable>
#include <mutex>
#include <queue>
#include <stdexcept>
#include <thread>
#include <vector>

namespace Parallel
{
	template <typename Type, typename Queue = std::queue<Type>>
	class Distributor {
		typedef typename Queue::size_type size_type;
		
		mutable std::mutex _mutex;
		mutable std::condition_variable _queue_ready;
		
		size_type _capacity;
		bool _done = false;
		std::vector<std::thread> _threads;
		Queue _queue;
		
	public:
		Distributor(size_type max_items_per_thread = 1, size_type thread_count = std::thread::hardware_concurrency()) : _capacity{thread_count * max_items_per_thread}
		{
			if (thread_count == 0)
				throw std::invalid_argument("Thread count must be non-zero");
			
			for (size_type index = 0; index < thread_count; index += 1)
				_threads.emplace_back(&Distributor::consume, this);
		}
		
		Distributor(Distributor &&) = default;
		Distributor &operator=(Distributor &&) = delete;

		~Distributor()
		{
			{
				std::lock_guard<std::mutex> guard(_mutex);
				_done = true;
				_queue_ready.notify_all();
			}
			
			for (auto && thread: _threads) thread.join();
		}
		
		size_type concurrency() const noexcept {return _threads.size();}
		size_type capacity() const noexcept {return _capacity;}
		size_type waiting() const noexcept {return _queue.size();}
		
		void operator()(Type &&value)
		{
			std::unique_lock<std::mutex> lock(_mutex);
			
			if (_capacity > 0)
				while (_queue.size() >= _capacity)
					_queue_ready.wait(lock);
			
			_queue.push(std::forward<Type>(value));
			_queue_ready.notify_one();
		}

	private:
		void consume()
		{
			std::unique_lock<std::mutex> lock(_mutex);
			
			while (true) {
				if (!_queue.empty()) {
					Type item{std::move(_queue.front())};
					_queue.pop();
					_queue_ready.notify_one();
					lock.unlock();
					item();
					lock.lock();
				} else if (_done) {
					break;
				} else {
					_queue_ready.wait(lock);
				}
			}
		}
	};
}