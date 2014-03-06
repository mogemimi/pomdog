//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include <gtest/iutest_switch.hpp>
#include <cstdint>
#include <Pomdog/Gameplay/GameWorld.hpp>
#include <Pomdog/Gameplay/GameObject.hpp>
#include <Pomdog/Gameplay2D/Camera2D.hpp>

using namespace Pomdog;

TEST(Camera2D, TrivialCase)
{
	GameObject gameObject;
	gameObject.AddComponent<Camera2D>();
	EXPECT_TRUE(gameObject.HasComponent<Camera2D>());
	EXPECT_NE(nullptr, gameObject.Component<Camera2D>());
}
