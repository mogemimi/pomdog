// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#pragma once

#include <vector>

namespace Pomdog {

class Skeleton;

namespace Spine {

class BoneDesc;

Skeleton CreateSkeleton(std::vector<BoneDesc> const& bones);

} // namespace Spine
} // namespace Pomdog
