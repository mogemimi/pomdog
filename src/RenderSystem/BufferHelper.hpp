// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include <cstdlib>

namespace Pomdog {
namespace Detail {
namespace BufferHelper {

std::size_t ToIndexElementOffsetBytes(IndexElementSize elementSize) noexcept;

} // namespace BufferHelper
} // namespace Detail
} // namespace Pomdog
