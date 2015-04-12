// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_DXGIFORMATHELPER_9FB57D4A_HPP
#define POMDOG_DXGIFORMATHELPER_9FB57D4A_HPP

#include <dxgi.h>
#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"

namespace Pomdog {
namespace Detail {
namespace RenderSystem {
namespace DXGI {

struct DXGIFormatHelper final {
    static DXGI_FORMAT ToDXGIFormat(DepthFormat format) noexcept;
    static DXGI_FORMAT ToDXGIFormat(SurfaceFormat format) noexcept;
    static DXGI_FORMAT ToDXGIFormat(IndexElementSize elementSize) noexcept;
};

} // namespace DXGI
} // namespace RenderSystem
} // namespace Detail
} // namespace Pomdog

#endif // POMDOG_DXGIFORMATHELPER_9FB57D4A_HPP
