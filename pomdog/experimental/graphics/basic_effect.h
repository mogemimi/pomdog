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
    Vector3 Position;

    /// vertex color
    Vector4 Color;
};

struct VertexPositionTexture final {
    /// position
    Vector3 Position;

    /// texture coordinates
    Vector2 TextureCoordinate;
};

struct VertexPositionTextureColor final {
    /// position
    Vector3 Position;

    /// texture coordinates
    Vector2 TextureCoordinate;

    /// vertex color
    Vector4 Color;
};

struct VertexPositionNormalColor final {
    /// position
    Vector3 Position;

    /// normalized surface normal
    Vector3 Normal;

    /// vertex color
    Vector4 Color;
};

struct VertexPositionNormalTexture final {
    /// position
    Vector3 Position;

    /// normalized surface normal
    Vector3 Normal;

    /// texture coordinates
    Vector2 TextureCoordinate;
};

struct alignas(16) ModelConstantBuffer final {
    Matrix4x4 Model;

    // {x___} = metalness (range: [0.0, 1.0])
    // {_yzw} = unused
    Vector4 Material;

    // {xyzw} = color.rgba
    Vector4 Color;

    void SetMetalness(float metalness) noexcept
    {
        Material.x = metalness;
    }
};

struct alignas(16) WorldConstantBuffer final {
    Matrix4x4 ViewProjection;

    Matrix4x4 View;

    Matrix4x4 Projection;

    Matrix4x4 InverseView;

    // {xyz_} = LightDirection.xyz
    // {___w} = unused
    Vector4 LightDirection;

    void SetLightDirection(const Vector3& lightDirection) noexcept
    {
        LightDirection.x = lightDirection.x;
        LightDirection.y = lightDirection.y;
        LightDirection.z = lightDirection.z;
    }
};

struct POMDOG_EXPORT BasicEffectDescription final {
    /// Enables lighting which requires a surface normal in the input layout.
    bool LightingEnabled = false;

    /// Enables use of texturing which requires a texture coordinates in the input layout.
    bool TextureEnabled = false;

    /// Enables use of a per-vertex color.
    bool VertexColorEnabled = false;
};

[[nodiscard]] POMDOG_EXPORT AssetBuilders::Builder<gpu::PipelineState>
CreateBasicEffect(AssetManager& assets, const BasicEffectDescription& desc);

} // namespace pomdog::BasicEffect
