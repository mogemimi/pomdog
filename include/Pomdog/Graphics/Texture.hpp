// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/Export.hpp"

namespace Pomdog {

class POMDOG_EXPORT Texture {
public:
    Texture() = default;
    Texture(Texture const&) = delete;
    Texture(Texture &&) = default;

    Texture & operator=(Texture const&) = delete;
    Texture & operator=(Texture &&) = default;

    virtual ~Texture() = default;
};

} // namespace Pomdog
