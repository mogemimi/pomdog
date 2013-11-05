//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include <iostream>
#include <pomdog/Math/Color.hpp>
#include <pomdog/Event/EventCategoryHelper.hpp>
#include <pomdog/Event/Event.hpp>
#include <pomdog/Event/EventConnection.hpp>
#include <pomdog/Event/EventQueue.hpp>

int main()
{
	using namespace Pomdog;

	EventQueue queue;
	
	auto connection = queue.Connect([](Event const& event){
		std::cout << "Hello, world." << std::to_string(event.GetCategoryID()) << std::endl;
	});
	
	auto event = std::make_shared<Event>(EventCategoryID(1234));
	
	queue.Enqueue(event);
	queue.Tick();

	return 0;
}
