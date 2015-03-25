// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_PNGTEXTUREREADER_C00C0E02_8156_45AD_80DA_DD3B4D89302B_HPP
#define POMDOG_PNGTEXTUREREADER_C00C0E02_8156_45AD_80DA_DD3B4D89302B_HPP

#if _MSC_VER > 1000
#pragma once
#endif

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

}// namespace Detail
}// namespace Pomdog

#endif // !defined(POMDOG_PNGTEXTUREREADER_C00C0E02_8156_45AD_80DA_DD3B4D89302B_HPP)
