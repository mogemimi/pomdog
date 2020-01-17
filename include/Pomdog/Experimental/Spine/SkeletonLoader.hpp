// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include <vector>

namespace Pomdog::Skeletal2D {
class Skeleton;
} // namespace Pomdog::Skeletal2D

namespace Pomdog::Spine {

class BoneDesc;

[[nodiscard]] Skeletal2D::Skeleton
CreateSkeleton(const std::vector<BoneDesc>& bones);

} // namespace Pomdog::Spine
