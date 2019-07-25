// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include <cstdint>
#include <cstdlib>

namespace Pomdog {
namespace Detail {
namespace BufferHelper {

std::size_t ToIndexElementOffsetBytes(IndexElementSize elementSize) noexcept;

std::uint16_t ToByteSize(InputElementFormat format) noexcept;

} // namespace BufferHelper
} // namespace Detail
} // namespace Pomdog
