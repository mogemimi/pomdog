// Copyright (c) 2013-2017 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog.Experimental/Rendering/RenderCommand.hpp"
#include "Pomdog.Experimental/Skeletal2D/Skeleton.hpp"
#include "Pomdog.Experimental/Skeletal2D/SkeletonTransform.hpp"
#include "Pomdog.Experimental/Skeletal2D/SkinnedMesh.hpp"
#include <Pomdog/Pomdog.hpp>
#include <memory>

namespace Pomdog {
namespace Detail {
namespace Rendering {

class SkinnedMeshCommand final : public RenderCommand {
public:
    std::type_index TypeIndex() const override;

    float DrawOrder() const override
    {
        return drawOrder;
    }

public:
    Matrix4x4 localToWorld;
    std::shared_ptr<Texture2D> texture;
    std::shared_ptr<SkinnedMesh> mesh;
    std::shared_ptr<Skeleton> skeleton;
    std::shared_ptr<SkeletonTransform> skeletonTransform;
    Color color;
    float drawOrder;
};

} // namespace Rendering
} // namespace Detail
} // namespace Pomdog
