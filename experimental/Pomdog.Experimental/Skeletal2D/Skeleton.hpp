// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_SKELETON_015B5D6C_HPP
#define POMDOG_SKELETON_015B5D6C_HPP

#include "Joint.hpp"
#include "JointPose.hpp"
#include <cstdint>
#include <vector>

namespace Pomdog {

class Skeleton {
public:
    Skeleton() = default;
    explicit Skeleton(std::vector<Joint> && joints);

    Skeleton(Skeleton const&) = delete;
    Skeleton & operator=(Skeleton const&) = delete;
    Skeleton(Skeleton &&) = default;
    Skeleton & operator=(Skeleton &&) = default;

    Joint const& Root() const;
    Joint const& Joints(JointIndex const& jointIndex) const;
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

}// namespace Pomdog

#endif // POMDOG_SKELETON_015B5D6C_HPP
