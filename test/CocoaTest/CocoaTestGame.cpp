//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "CocoaTestGame.hpp"

namespace Pomdog {

static int a = 0;

void CocoaTestGame::Initialize()
{
	//
	a += 100;
}

void CocoaTestGame::Update()
{
	//
	a += 2;
}

void CocoaTestGame::Draw()
{
	//
	a += 3;
}

}// namespace Pomdog
