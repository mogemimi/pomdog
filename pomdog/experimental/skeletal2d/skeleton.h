// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"
#include "pomdog/experimental/skeletal2d/joint.h"
#include "pomdog/experimental/skeletal2d/joint_pose.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::skeletal2d {

class Skeleton final {
public:
    Skeleton() = default;
    explicit Skeleton(std::vector<Joint>&& joints);

    Skeleton(const Skeleton&) = delete;
    Skeleton& operator=(const Skeleton&) = delete;
    Skeleton(Skeleton&&) = default;
    Skeleton& operator=(Skeleton&&) = default;

    const Joint& root() const;
    const Joint& joints(const JointIndex& jointIndex) const;
    u16 jointCount() const;

    using const_iterator = std::vector<Joint>::const_iterator;
    using iterator = std::vector<Joint>::iterator;

    const_iterator begin() const;
    const_iterator end() const;
    iterator begin();
    iterator end();

private:
    std::vector<Joint> joints_;
};

} // namespace pomdog::skeletal2d
