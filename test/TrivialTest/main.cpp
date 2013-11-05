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

namespace Pomdog {
namespace Details {

}// namespace Details
}// namespace Pomdog

int main()
{
	for (auto i = 0; i < 3; ++i) {
		auto id = Pomdog::EventCategoryHelper::CreateCategoryID("trivial event");
		std::cout << "trivial event => " << id.value << std::endl;
	}
	
	auto id = Pomdog::EventCategoryHelper::CreateCategoryID("Trivial Event");
	std::cout << "Trivial Event => " << id.value << std::endl;
	
	id = Pomdog::EventCategoryHelper::CreateCategoryID("trivial_event");
	std::cout << "trivial_event => " << id.value << std::endl;

	Pomdog::Color color {1, 1, 1, 1};

	std::cout << color.r << ", " << color.g << ", " << color.b << std::endl;
	std::cout << "Hello, world." << std::endl;

	char a;
	std::cin >> a;

	return 0;
}
