// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#pragma once

#include <Pomdog/Pomdog.hpp>
#include <cstdint>
#include <cstddef>
#include <memory>

namespace Pomdog {

class SkinnedEffect {
public:
    explicit SkinnedEffect(GraphicsDevice & graphicsDevice,
        AssetManager & assets);

    ~SkinnedEffect();

    void SetWorldViewProjection(Matrix4x4 const& worldViewProjection);

    void SetTexture(std::shared_ptr<Texture2D> const& texture);

    void SetColor(Color const& color);

    void SetBoneTransforms(Matrix3x2 const* boneTransforms, std::size_t count);

    void Apply(GraphicsContext & graphicsContext);

    static constexpr size_t MaxBones = 96;

private:
    class Impl;
    std::unique_ptr<Impl> impl;
};

} // namespace Pomdog
