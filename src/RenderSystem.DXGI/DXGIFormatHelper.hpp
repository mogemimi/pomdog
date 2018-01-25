// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

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
