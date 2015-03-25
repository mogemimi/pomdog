// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#include "SpriteCommand.hpp"
#include <typeinfo>

namespace Pomdog {
namespace Detail {
namespace Rendering {
//-----------------------------------------------------------------------
std::type_index SpriteCommand::TypeIndex() const
{
    static const std::type_index index = typeid(SpriteCommand);
    return index;
}
//-----------------------------------------------------------------------
}// namespace Rendering
}// namespace Detail
}// namespace Pomdog
