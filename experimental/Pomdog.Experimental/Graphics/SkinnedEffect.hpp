// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

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

    void SetWorldViewProjection(const Matrix4x4& worldViewProjection);

    void SetTexture(const std::shared_ptr<Texture2D>& texture);

    void SetColor(const Color& color);

    void SetBoneTransforms(Matrix3x2 const* boneTransforms, std::size_t count);

    void Apply(GraphicsCommandQueue & commandQueue);

    static constexpr size_t MaxBones = 96;

private:
    class Impl;
    std::unique_ptr<Impl> impl;
};

} // namespace Pomdog
