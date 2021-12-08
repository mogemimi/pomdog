// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/ecs/component_type_index.hpp"
#include "pomdog/utility/assert.hpp"

namespace pomdog::ecs::detail {

ComponentTypeIndex::IndexType ComponentTypeIndex::count = 0;

ComponentTypeIndex::IndexType ComponentTypeIndex::IncrementIndex()
{
    POMDOG_ASSERT(count < std::numeric_limits<IndexType>::max());
    ++count;
    POMDOG_ASSERT(count > 0);
    return count;
}

} // namespace pomdog::ecs::detail
