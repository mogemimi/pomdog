// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include <cstdint>

namespace Pomdog::MagicaVoxel {

struct VoxChunkHeader final {
    std::int32_t ID;
    std::int32_t ContentSize;
    std::int32_t ChildrenSize;
};

} // namespace Pomdog::MagicaVoxel
