// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include <cstdint>
#include <cstddef>
#include <memory>

namespace Pomdog {
namespace Detail {

namespace DDSTextureReader {

std::shared_ptr<Texture2D> Read(std::shared_ptr<GraphicsDevice> const& graphicsDevice,
    std::uint8_t const* data, std::size_t byteLength);

} // namespace DDSTextureReader

} // namespace Detail
} // namespace Pomdog
