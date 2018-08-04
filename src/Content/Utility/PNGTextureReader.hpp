// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include <cstddef>
#include <cstdint>
#include <memory>

namespace Pomdog {
namespace Detail {

struct PNGTextureReader {
    static std::shared_ptr<Texture2D> Read(
        const std::shared_ptr<GraphicsDevice>& graphicsDevice,
        const std::uint8_t* data,
        std::size_t byteLength);
};

} // namespace Detail
} // namespace Pomdog
