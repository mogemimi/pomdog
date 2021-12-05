// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/ConditionalCompilation.hpp"
#include "Pomdog/Experimental/Skeletal2D/JointPose.hpp"
#include "Pomdog/Math/Matrix3x2.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog::Skeletal2D {

class Skeleton;

class SkeletonPose final {
public:
    std::vector<JointPose> JointPoses;

    static SkeletonPose CreateBindPose(const Skeleton& skeleton);
};

} // namespace Pomdog::Skeletal2D
