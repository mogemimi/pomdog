// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/ConditionalCompilation.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstdint>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog::MagicaVoxel {

struct VoxChunkHeader final {
    std::int32_t ID;
    std::int32_t ContentSize;
    std::int32_t ChildrenSize;
};

} // namespace Pomdog::MagicaVoxel
