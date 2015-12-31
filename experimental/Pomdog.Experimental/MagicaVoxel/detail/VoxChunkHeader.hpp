// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#pragma once

#include <cstdint>

namespace Pomdog {
namespace MagicaVoxel {

struct VoxChunkHeader {
    std::int32_t ID;
    std::int32_t ContentSize;
    std::int32_t ChildrenSize;
};

} // namespace MagicaVoxel
} // namespace Pomdog
