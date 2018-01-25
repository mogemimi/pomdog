// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include "ParticleBatchCommand.hpp"
#include <typeinfo>

namespace Pomdog {
namespace Detail {
namespace Rendering {

std::type_index ParticleBatchCommand::TypeIndex() const
{
    static const std::type_index index = typeid(ParticleBatchCommand);
    return index;
}

} // namespace Rendering
} // namespace Detail
} // namespace Pomdog
