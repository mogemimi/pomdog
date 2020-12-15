// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Graphics/ForwardDeclarations.hpp"
#include <dxgi.h>

namespace Pomdog::Detail::DXGI {

[[nodiscard]] DXGI_FORMAT
ToDXGIFormat(SurfaceFormat format) noexcept;

[[nodiscard]] DXGI_FORMAT
ToDXGIFormat(IndexElementSize elementSize) noexcept;

[[nodiscard]] DXGI_FORMAT
ToDXGIFormat(InputElementFormat format) noexcept;

} // namespace Pomdog::Detail::DXGI
