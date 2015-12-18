// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#ifndef POMDOG_DXGIFORMATHELPER_9FB57D4A_HPP
#define POMDOG_DXGIFORMATHELPER_9FB57D4A_HPP

#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include <dxgi.h>

namespace Pomdog {
namespace Detail {
namespace DXGI {

struct DXGIFormatHelper final {
    static DXGI_FORMAT ToDXGIFormat(DepthFormat format) noexcept;
    static DXGI_FORMAT ToDXGIFormat(SurfaceFormat format) noexcept;
    static DXGI_FORMAT ToDXGIFormat(IndexElementSize elementSize) noexcept;
    static DXGI_FORMAT ToDXGIFormat(InputElementFormat format) noexcept;
};

} // namespace DXGI
} // namespace Detail
} // namespace Pomdog

#endif // POMDOG_DXGIFORMATHELPER_9FB57D4A_HPP
