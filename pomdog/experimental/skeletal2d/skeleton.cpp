// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/skeletal2d/skeleton.h"
#include "pomdog/utility/assert.h"

namespace pomdog::skeletal2d {

Skeleton::Skeleton(std::vector<Joint>&& jointsIn)
    : joints(std::move(jointsIn))
{
    POMDOG_ASSERT(!joints.empty());
    POMDOG_ASSERT(!joints.front().Parent);
    POMDOG_ASSERT(joints.front().Index);
    POMDOG_ASSERT(*joints.front().Index == 0);
}

const Joint& Skeleton::Root() const
{
    POMDOG_ASSERT(!joints.empty());
    return joints.front();
}

const Joint& Skeleton::Joints(const JointIndex& jointIndex) const
{
    POMDOG_ASSERT(jointIndex);
    POMDOG_ASSERT(*jointIndex < joints.size());
    return joints[*jointIndex];
}

std::uint16_t Skeleton::JointCount() const
{
    POMDOG_ASSERT(joints.size() < std::numeric_limits<std::uint16_t>::max());
    return static_cast<std::uint16_t>(joints.size());
}

Skeleton::const_iterator Skeleton::begin() const
{
    return std::begin(joints);
}

Skeleton::const_iterator Skeleton::end() const
{
    return std::end(joints);
}

Skeleton::iterator Skeleton::begin()
{
    return std::begin(joints);
}

Skeleton::iterator Skeleton::end()
{
    return std::end(joints);
}

} // namespace pomdog::skeletal2d
