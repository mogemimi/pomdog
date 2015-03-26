// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_SKELETONTRANSFORM_B5648A89_HPP
#define POMDOG_SKELETONTRANSFORM_B5648A89_HPP

#include "SkeletonPose.hpp"
#include "Pomdog/Math/Matrix3x2.hpp"
#include <vector>

namespace Pomdog {

class SkeletonTransform {
public:
    SkeletonPose Pose;
    std::vector<Matrix3x2> GlobalPose;
};

}// namespace Pomdog

#endif // POMDOG_SKELETONTRANSFORM_B5648A89_HPP
