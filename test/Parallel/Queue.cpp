//
//  Queue.cpp
//  This file is part of the "Parallel" project and released under the .
//
//  Created by Samuel Williams on 23/2/2019.
//  Copyright, 2019, by Samuel Williams. All rights reserved.
//

#include <UnitTest/UnitTest.hpp>

#include <Parallel/Queue.hpp>

namespace Parallel
{
	UnitTest::Suite QueueTestSuite {
		"Parallel::Queue",
		
		{"it can add and remove items",
			[](UnitTest::Examiner & examiner) {
				Queue<int> queue;
				
				queue.enqueue(10);
				
				auto & items = queue.dequeue();
				
				examiner.expect(items.size()) == 1;
				examiner.expect(items.at(0)) == 10;
			}
		},
	};
}
