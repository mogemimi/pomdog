// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/Export.hpp"

namespace Pomdog {

class POMDOG_EXPORT Game {
public:
    Game() = default;
    Game(Game const&) = delete;
    Game & operator=(Game const&) = delete;

    virtual ~Game() = default;

    virtual void Initialize() = 0;

    virtual void Update() = 0;

    virtual void Draw() = 0;
};

} // namespace Pomdog
