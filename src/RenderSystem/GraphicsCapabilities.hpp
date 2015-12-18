// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#pragma once

#include <cstddef>

namespace Pomdog {
namespace Detail {

struct GraphicsCapabilities {
    std::size_t ConstantBufferSlotCount;
    std::size_t SamplerSlotCount;
};

} // namespace Detail
} // namespace Pomdog
