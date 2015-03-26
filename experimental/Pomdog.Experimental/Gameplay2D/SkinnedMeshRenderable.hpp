// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_SKINNEDMESHRENDERABLE_6584A6D1_HPP
#define POMDOG_SKINNEDMESHRENDERABLE_6584A6D1_HPP

#include "Renderable.hpp"
#include "Pomdog.Experimental/Rendering/Commands/SkinnedMeshCommand.hpp"
#include "Pomdog.Experimental/Graphics/PolygonBatch.hpp"
#include <Pomdog/Pomdog.hpp>
#include <memory>

namespace Pomdog {

class SkinnedMeshRenderable: public Renderable {
public:
    SkinnedMeshRenderable(
        std::shared_ptr<Skeleton> const& skeleton,
        std::shared_ptr<SkeletonTransform> const& skeletonTransform,
        std::shared_ptr<SkinnedMesh> const& mesh,
        std::shared_ptr<Texture2D> const& texture);

    void Visit(GameObject & gameObject, Renderer & renderer) override;

private:
    Detail::Rendering::SkinnedMeshCommand command;
};

}// namespace Pomdog

#endif // POMDOG_SKINNEDMESHRENDERABLE_6584A6D1_HPP
