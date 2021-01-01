// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/ConditionalCompilation.hpp"
#include "Pomdog/Graphics/ForwardDeclarations.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <dxgi.h>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog::Detail::DXGI {

[[nodiscard]] DXGI_FORMAT
ToDXGIFormat(SurfaceFormat format) noexcept;

[[nodiscard]] DXGI_FORMAT
ToDXGIFormat(IndexElementSize elementSize) noexcept;

[[nodiscard]] DXGI_FORMAT
ToDXGIFormat(InputElementFormat format) noexcept;

} // namespace Pomdog::Detail::DXGI
