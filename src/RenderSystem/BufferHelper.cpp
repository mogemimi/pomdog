// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#include "BufferHelper.hpp"
#include "Pomdog/Graphics/IndexElementSize.hpp"

namespace Pomdog {
namespace Detail {
namespace BufferHelper {

std::size_t ToIndexElementOffsetBytes(IndexElementSize elementSize) noexcept
{
    switch (elementSize) {
    case IndexElementSize::ThirtyTwoBits: return 4;
    case IndexElementSize::SixteenBits: return 2;
    };
#ifdef _MSC_VER
    // FUS RO DAH!
    return 4;
#endif
}

} // namespace BufferHelper
} // namespace Detail
} // namespace Pomdog
