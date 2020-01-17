// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include <cstdint>
#include <cstdlib>

namespace Pomdog::Detail::BufferHelper {

std::size_t ToIndexElementOffsetBytes(IndexElementSize elementSize) noexcept;

std::uint16_t ToByteSize(InputElementFormat format) noexcept;

} // namespace Pomdog::Detail::BufferHelper
