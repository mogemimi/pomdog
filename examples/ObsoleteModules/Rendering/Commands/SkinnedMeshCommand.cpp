// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#include "SkinnedMeshCommand.hpp"
#include <typeinfo>

namespace Pomdog {
namespace Detail {
namespace Rendering {

std::type_index SkinnedMeshCommand::TypeIndex() const
{
    static const std::type_index index = typeid(SkinnedMeshCommand);
    return index;
}

} // namespace Rendering
} // namespace Detail
} // namespace Pomdog
