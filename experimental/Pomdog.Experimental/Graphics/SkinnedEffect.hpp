// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#pragma once

#include <Pomdog/Pomdog.hpp>
#include <cstddef>
#include <cstdint>
#include <memory>

namespace Pomdog {

class SkinnedEffect {
public:
    explicit SkinnedEffect(GraphicsDevice& graphicsDevice,
        AssetManager& assets);

    ~SkinnedEffect();

    void SetWorldViewProjection(const Matrix4x4& worldViewProjection);

    void SetTexture(const std::shared_ptr<Texture2D>& texture);

    void SetColor(const Color& color);

    void SetBoneTransforms(Matrix3x2 const* boneTransforms, std::size_t count);

    void Apply(GraphicsCommandQueue& commandQueue);

    static constexpr size_t MaxBones = 96;

private:
    class Impl;
    std::unique_ptr<Impl> impl;
};

} // namespace Pomdog
