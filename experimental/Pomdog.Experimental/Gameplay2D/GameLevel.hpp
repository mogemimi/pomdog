// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include <Pomdog/Pomdog.hpp>

namespace Pomdog {

class GameHost;

class GameLevel {
public:
    GameLevel() = default;
    GameLevel(GameLevel const&) = delete;
    GameLevel & operator=(GameLevel const&) = delete;

    virtual ~GameLevel() = default;

    virtual void Update(GameHost & gameHost, EntityManager & manager) = 0;
};

} // namespace Pomdog
