// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#ifndef POMDOG_PNGTEXTUREREADER_C00C0E02_HPP
#define POMDOG_PNGTEXTUREREADER_C00C0E02_HPP

#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include <cstdint>
#include <cstddef>
#include <memory>

namespace Pomdog {
namespace Detail {

struct PNGTextureReader {
    static std::shared_ptr<Texture2D> Read(
        std::shared_ptr<GraphicsDevice> const& graphicsDevice,
        std::uint8_t const* data, std::size_t byteLength);
};

} // namespace Detail
} // namespace Pomdog

#endif // POMDOG_PNGTEXTUREREADER_C00C0E02_HPP
