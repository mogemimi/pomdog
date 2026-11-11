// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/skeletal2d/skeleton.h"
#include "pomdog/basic/types.h"
#include "pomdog/utility/assert.h"

namespace pomdog::skeletal2d {

Skeleton::Skeleton(std::vector<Joint>&& jointsIn)
    : joints_(std::move(jointsIn))
{
    POMDOG_ASSERT(!joints_.empty());
    POMDOG_ASSERT(!joints_.front().parent);
    POMDOG_ASSERT(joints_.front().index);
    POMDOG_ASSERT(*joints_.front().index == 0);
}

const Joint& Skeleton::root() const
{
    POMDOG_ASSERT(!joints_.empty());
    return joints_.front();
}

const Joint& Skeleton::joints(const JointIndex& jointIndex) const
{
    POMDOG_ASSERT(jointIndex);
    POMDOG_ASSERT(*jointIndex < joints_.size());
    return joints_[*jointIndex];
}

u16 Skeleton::jointCount() const
{
    POMDOG_ASSERT(joints_.size() < std::numeric_limits<u16>::max());
    return static_cast<u16>(joints_.size());
}

Skeleton::const_iterator Skeleton::begin() const
{
    return std::begin(joints_);
}

Skeleton::const_iterator Skeleton::end() const
{
    return std::end(joints_);
}

Skeleton::iterator Skeleton::begin()
{
    return std::begin(joints_);
}

Skeleton::iterator Skeleton::end()
{
    return std::end(joints_);
}

} // namespace pomdog::skeletal2d
