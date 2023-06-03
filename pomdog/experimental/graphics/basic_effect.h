// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/export.h"
#include "pomdog/content/asset_builders/builder.h"
#include "pomdog/gpu/forward_declarations.h"
#include "pomdog/math/matrix4x4.h"
#include "pomdog/math/vector2.h"
#include "pomdog/math/vector3.h"
#include "pomdog/math/vector4.h"

namespace pomdog {
class AssetManager;
} // namespace pomdog

namespace pomdog::BasicEffect {

struct VertexPositionColor final {
    /// position
    Vector3 position;

    /// vertex color
    Vector4 color;
};

struct VertexPositionTexture final {
    /// position
    Vector3 position;

    /// texture coordinates
    Vector2 textureCoordinate;
};

struct VertexPositionTextureColor final {
    /// position
    Vector3 position;

    /// texture coordinates
    Vector2 textureCoordinate;

    /// vertex color
    Vector4 color;
};

struct VertexPositionNormalColor final {
    /// position
    Vector3 position;

    /// normalized surface normal
    Vector3 normal;

    /// vertex color
    Vector4 color;
};

struct VertexPositionNormalTexture final {
    /// position
    Vector3 position;

    /// normalized surface normal
    Vector3 normal;

    /// texture coordinates
    Vector2 textureCoordinate;
};

struct alignas(16) ModelConstantBuffer final {
    Matrix4x4 model;

    // {x___} = metalness (range: [0.0, 1.0])
    // {_yzw} = unused
    Vector4 material;

    // {xyzw} = color.rgba
    Vector4 color;

    void SetMetalness(float metalness) noexcept
    {
        material.x = metalness;
    }
};

struct alignas(16) WorldConstantBuffer final {
    Matrix4x4 viewProjection;

    Matrix4x4 view;

    Matrix4x4 projection;

    Matrix4x4 inverseView;

    // {xyz_} = LightDirection.xyz
    // {___w} = unused
    Vector4 lightDirection;

    void setLightDirection(const Vector3& lightDirectionIn) noexcept
    {
        lightDirection.x = lightDirectionIn.x;
        lightDirection.y = lightDirectionIn.y;
        lightDirection.z = lightDirectionIn.z;
    }
};

struct POMDOG_EXPORT BasicEffectDescription final {
    /// Enables lighting which requires a surface normal in the input layout.
    bool lightingEnabled = false;

    /// Enables use of texturing which requires a texture coordinates in the input layout.
    bool textureEnabled = false;

    /// Enables use of a per-vertex color.
    bool vertexColorEnabled = false;
};

[[nodiscard]] POMDOG_EXPORT AssetBuilders::Builder<gpu::PipelineState>
createBasicEffect(AssetManager& assets, const BasicEffectDescription& desc);

} // namespace pomdog::BasicEffect
