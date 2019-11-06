// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#pragma once

#include "GraphicsComponent.hpp"
#include "Pomdog.Experimental/Graphics/PrimitiveBatch.hpp"
#include "Pomdog.Experimental/Rendering/Commands/SkinnedMeshCommand.hpp"
#include <Pomdog/Pomdog.hpp>
#include <memory>

namespace Pomdog {

class SkinnedMeshRenderable : public GraphicsComponent {
public:
    SkinnedMeshRenderable(
        const std::shared_ptr<Skeleton>& skeleton,
        const std::shared_ptr<SkeletonTransform>& skeletonTransform,
        const std::shared_ptr<SkinnedMesh>& mesh,
        const std::shared_ptr<Texture2D>& texture);

    void Visit(Entity& entity, Renderer& renderer) override;

private:
    Detail::Rendering::SkinnedMeshCommand command;
};

} // namespace Pomdog
