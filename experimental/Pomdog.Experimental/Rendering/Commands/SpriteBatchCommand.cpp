// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include "Pomdog.Experimental/Rendering/Commands/SpriteBatchCommand.hpp"
#include <typeinfo>

namespace Pomdog {
namespace Rendering {

std::type_index SpriteRenderCommand::GetType() const noexcept
{
    static const std::type_index index = typeid(SpriteBatchCommand);
    return index;
}

void SpriteBatchCommand::Execute(SpriteBatchRenderer & spriteBatch)
{
    spriteBatch.Draw(texture, position, sourceRect, color, rotation, originPivot, scale);
}

void SpriteFontCommand::Execute(SpriteBatchRenderer& spriteBatch)
{
    POMDOG_ASSERT(spriteFont);
    spriteFont->Draw(spriteBatch, text, position, color, rotation, scale);
}

} // namespace Rendering
} // namespace Pomdog
