// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/experimental/skeletal2d/joint.hpp"
#include "pomdog/experimental/skeletal2d/joint_pose.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstdint>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog::Skeletal2D {

class Skeleton final {
public:
    Skeleton() = default;
    explicit Skeleton(std::vector<Joint>&& joints);

    Skeleton(const Skeleton&) = delete;
    Skeleton& operator=(const Skeleton&) = delete;
    Skeleton(Skeleton&&) = default;
    Skeleton& operator=(Skeleton&&) = default;

    const Joint& Root() const;
    const Joint& Joints(const JointIndex& jointIndex) const;
    std::uint16_t JointCount() const;

    using const_iterator = std::vector<Joint>::const_iterator;
    using iterator = std::vector<Joint>::iterator;

    const_iterator begin() const;
    const_iterator end() const;
    iterator begin();
    iterator end();

private:
    std::vector<Joint> joints;
};

} // namespace Pomdog::Skeletal2D
