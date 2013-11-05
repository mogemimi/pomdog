//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include <iostream>
#include <pomdog/Math/Color.hpp>
#include <pomdog/Event/EventCodeHelper.hpp>
#include <pomdog/Event/Event.hpp>
#include <pomdog/Event/EventConnection.hpp>
#include <pomdog/Event/EventQueue.hpp>
#include <pomdog/GamePlay/GameObject.hpp>

int main()
{
	using namespace Pomdog;

	EventQueue queue;
	
	auto connection = queue.Connect([](Event const& event) {
		std::cout << "event: " << std::to_string(event.GetCategoryID()) << std::endl;
	});
	
	queue.Enqueue(std::make_shared<Event>(EventCode(1234)));
	queue.Enqueue(std::make_shared<Event>(EventCode(2345)));
	queue.Enqueue(std::make_shared<Event>(EventCode(3456)));
	queue.Tick();
	
	connection.Disconnect();
	queue.Enqueue(std::make_shared<Event>(EventCode(7890)));
	queue.Tick();
	
	struct Transform
	{
		float x, y, z;
	};
	GameObject gameObject;
	gameObject.AddComponent(Transform{1, 2, 3});
	
	std::cout << "has component? " << gameObject.HasComponent<Transform>() << std::endl;
	
	if (auto transform = gameObject.GetComponent<Transform>()) {
		std::cout << "x = " << transform->x << std::endl;
	}
	
	gameObject.RemoveComponent<Transform>();
	
	std::cout << "has component? " << gameObject.HasComponent<Transform>() << std::endl;
	
	return 0;
}
