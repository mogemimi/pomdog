// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#include "PrimitiveCommand.hpp"
#include <typeinfo>

namespace Pomdog {
namespace Detail {
namespace Rendering {
//-----------------------------------------------------------------------
std::type_index PrimitiveCommand::TypeIndex() const
{
    static const std::type_index index = typeid(PrimitiveCommand);
    return index;
}
//-----------------------------------------------------------------------
} // namespace Rendering
} // namespace Detail
} // namespace Pomdog
