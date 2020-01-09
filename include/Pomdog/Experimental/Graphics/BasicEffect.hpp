// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Content/AssetBuilders/Builder.hpp"
#include "Pomdog/Math/Matrix4x4.hpp"
#include "Pomdog/Math/Vector2.hpp"
#include "Pomdog/Math/Vector3.hpp"
#include "Pomdog/Math/Vector4.hpp"

namespace Pomdog {
class AssetManager;
class PipelineState;
} // namespace Pomdog

namespace Pomdog::BasicEffect {

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
        Material.X = metalness;
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
        LightDirection.X = lightDirection.X;
        LightDirection.Y = lightDirection.Y;
        LightDirection.Z = lightDirection.Z;
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

[[nodiscard]] POMDOG_EXPORT
AssetBuilders::Builder<PipelineState>
CreateBasicEffect(AssetManager& assets, const BasicEffectDescription& desc);

} // namespace Pomdog::BasicEffect
