//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include <iostream>
#include <pomdog/Math/Color.hpp>

namespace Pomdog {
namespace Details {

}// namespace Details
}// namespace Pomdog

int main()
{
	Pomdog::Color color {1, 1, 1, 1};

	std::cout << color.r << ", " << color.g << ", " << color.b << std::endl;
	std::cout << "Hello, world." << std::endl;

	char a;
	std::cin >> a;

	return 0;
}
