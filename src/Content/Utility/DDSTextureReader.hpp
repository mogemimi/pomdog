// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_DDSTEXTUREREADER_0C8072DF_HPP
#define POMDOG_DDSTEXTUREREADER_0C8072DF_HPP

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

#endif // POMDOG_DDSTEXTUREREADER_0C8072DF_HPP
