// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/export.h"

namespace pomdog::gpu {

class POMDOG_EXPORT Texture {
public:
    Texture() = default;
    Texture(const Texture&) = delete;
    Texture(Texture&&) = default;

    Texture& operator=(const Texture&) = delete;
    Texture& operator=(Texture&&) = default;

    virtual ~Texture();
};

} // namespace pomdog::gpu
