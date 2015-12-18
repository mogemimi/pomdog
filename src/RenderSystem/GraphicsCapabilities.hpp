// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#ifndef POMDOG_GRAPHICSCAPABILITIES_17FEAA5D_HPP
#define POMDOG_GRAPHICSCAPABILITIES_17FEAA5D_HPP

#include <cstddef>

namespace Pomdog {
namespace Detail {

struct GraphicsCapabilities {
    std::size_t ConstantBufferSlotCount;
    std::size_t SamplerSlotCount;
};

} // namespace Detail
} // namespace Pomdog

#endif // POMDOG_GRAPHICSCAPABILITIES_17FEAA5D_HPP
