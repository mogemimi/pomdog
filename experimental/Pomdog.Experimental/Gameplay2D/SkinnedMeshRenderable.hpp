// Copyright (c) 2013-2017 mogemimi. Distributed under the MIT license.

#pragma once

#include "GraphicsComponent.hpp"
#include "Pomdog.Experimental/Rendering/Commands/SkinnedMeshCommand.hpp"
#include "Pomdog.Experimental/Graphics/PolygonBatch.hpp"
#include <Pomdog/Pomdog.hpp>
#include <memory>

namespace Pomdog {

class SkinnedMeshRenderable : public GraphicsComponent {
public:
    SkinnedMeshRenderable(
        std::shared_ptr<Skeleton> const& skeleton,
        std::shared_ptr<SkeletonTransform> const& skeletonTransform,
        std::shared_ptr<SkinnedMesh> const& mesh,
        std::shared_ptr<Texture2D> const& texture);

    void Visit(Entity & entity, Renderer & renderer) override;

private:
    Detail::Rendering::SkinnedMeshCommand command;
};

} // namespace Pomdog
