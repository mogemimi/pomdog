// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_GAMELEVEL_20FE69E2_A4EB_4800_AFA8_3B039147EDFA_HPP
#define POMDOG_GAMELEVEL_20FE69E2_A4EB_4800_AFA8_3B039147EDFA_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include <Pomdog/Pomdog.hpp>

namespace Pomdog {

class GameHost;

class GameLevel {
public:
    GameLevel() = default;
    GameLevel(GameLevel const&) = delete;
    GameLevel & operator=(GameLevel const&) = delete;

    virtual ~GameLevel() = default;

    virtual void Update(GameHost & gameHost, GameWorld & world) = 0;
};

}// namespace Pomdog

#endif // !defined(POMDOG_GAMELEVEL_20FE69E2_A4EB_4800_AFA8_3B039147EDFA_HPP)
