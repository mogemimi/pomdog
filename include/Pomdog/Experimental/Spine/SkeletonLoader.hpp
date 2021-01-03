// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/ConditionalCompilation.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog::Skeletal2D {
class Skeleton;
} // namespace Pomdog::Skeletal2D

namespace Pomdog::Spine {

class BoneDesc;

[[nodiscard]] Skeletal2D::Skeleton
CreateSkeleton(const std::vector<BoneDesc>& bones);

} // namespace Pomdog::Spine
