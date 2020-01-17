// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Experimental/Skeletal2D/Joint.hpp"
#include "Pomdog/Experimental/Skeletal2D/JointPose.hpp"
#include <cstdint>
#include <vector>

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

    typedef std::vector<Joint>::const_iterator const_iterator;
    typedef std::vector<Joint>::iterator iterator;

    const_iterator begin() const;
    const_iterator end() const;
    iterator begin();
    iterator end();

private:
    std::vector<Joint> joints;
};

} // namespace Pomdog::Skeletal2D
