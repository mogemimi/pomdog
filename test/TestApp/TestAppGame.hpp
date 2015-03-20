// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_COCOATESTGAME_HPP
#define POMDOG_COCOATESTGAME_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include <Pomdog/Pomdog.hpp>

namespace TestApp {

using namespace Pomdog;

class TestAppGame: public Game {
public:
	explicit TestAppGame(std::shared_ptr<GameHost> const& gameHost);

	~TestAppGame();

	void Initialize();
	
	void Update();
	
	void Draw();

private:
	std::shared_ptr<GameHost> gameHost;
	std::unique_ptr<Game> game;
};

}// namespace TestApp

#endif // !defined(POMDOG_COCOATESTGAME_HPP)
