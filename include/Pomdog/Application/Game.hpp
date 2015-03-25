// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_GAME_8B7C90B7_0C6E_4161_9C57_2BDE1DC6DD57_HPP
#define POMDOG_GAME_8B7C90B7_0C6E_4161_9C57_2BDE1DC6DD57_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include "Pomdog/Basic/Export.hpp"

namespace Pomdog {

class POMDOG_EXPORT Game {
public:
	Game() = default;
	Game(Game const&) = delete;
	Game & operator=(Game const&) = delete;

	virtual ~Game() = default;

	virtual void Initialize() = 0;

	virtual bool CompleteInitialize() const { return true; }

	virtual void Update() = 0;

	virtual void Draw() = 0;
};

}// namespace Pomdog

#endif // !defined(POMDOG_GAME_8B7C90B7_0C6E_4161_9C57_2BDE1DC6DD57_HPP)
