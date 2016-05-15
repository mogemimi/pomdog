// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/Export.hpp"

namespace Pomdog {

class MouseState;

class POMDOG_EXPORT Mouse {
public:
    Mouse() = default;
    Mouse(const Mouse&) = delete;
    Mouse & operator=(const Mouse&) = delete;

    virtual ~Mouse() = default;

    virtual MouseState GetState() const = 0;
};

} // namespace Pomdog
