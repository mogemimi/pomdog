// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#include "SpriteBatchCommand.hpp"
#include <typeinfo>

namespace Pomdog {
namespace Rendering {

std::type_index SpriteBatchCommand::GetType() const noexcept
{
    static const std::type_index index = typeid(SpriteBatchCommand);
    return index;
}

float SpriteBatchCommand::GetDrawOrder() const noexcept
{
    return drawOrder;
}

} // namespace Rendering
} // namespace Pomdog
