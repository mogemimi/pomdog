// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_DXGIFORMATHELPER_9FB57D4A_HPP
#define POMDOG_DXGIFORMATHELPER_9FB57D4A_HPP

#include "PrerequisitesDirect3D11.hpp"
#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"

namespace Pomdog {
namespace Detail {
namespace RenderSystem {
namespace Direct3D11 {

struct DXGIFormatHelper final {
    static DXGI_FORMAT ToDXGIFormat(DepthFormat format);
    static DXGI_FORMAT ToDXGIFormat(SurfaceFormat format);
};

} // namespace Direct3D11
} // namespace RenderSystem
} // namespace Detail
} // namespace Pomdog

#endif // POMDOG_DXGIFORMATHELPER_9FB57D4A_HPP
