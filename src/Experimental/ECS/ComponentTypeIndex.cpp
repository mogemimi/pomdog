// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#include "Pomdog/Experimental/ECS/ComponentTypeIndex.hpp"
#include "Pomdog/Utility/Assert.hpp"

namespace Pomdog {
namespace Detail {
namespace Gameplay {

ComponentTypeIndex::IndexType ComponentTypeIndex::count = 0;

ComponentTypeIndex::IndexType ComponentTypeIndex::IncrementIndex()
{
    POMDOG_ASSERT(count < std::numeric_limits<IndexType>::max());
    ++count;
    POMDOG_ASSERT(count > 0);
    return count;
}

} // namespace Gameplay
} // namespace Detail
} // namespace Pomdog
