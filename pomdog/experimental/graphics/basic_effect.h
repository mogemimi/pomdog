// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/basic/types.h"
#include "pomdog/math/matrix4x4.h"
#include "pomdog/math/vector2.h"
#include "pomdog/math/vector3.h"
#include "pomdog/math/vector4.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <tuple>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu {
class GraphicsDevice;
} // namespace pomdog::gpu

namespace pomdog::vfs {
class FileSystemContext;
} // namespace pomdog::vfs

namespace pomdog {
class Error;
class PipelineStateBuilder;
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

    void SetMetalness(f32 metalness) noexcept
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

/// Specifies the variant of the basic effect shader.
enum class BasicEffectVariant : u8 {
    /// Uses position and vertex color.
    PositionColor,

    /// Uses position and texture coordinates.
    PositionTexture,

    /// Uses position, normal, and vertex color with lighting.
    PositionNormalColor,

    /// Uses position, normal, and texture coordinates with lighting.
    PositionNormalTexture,
};

/// Creates a pipeline state builder for the specified basic effect variant.
[[nodiscard]] POMDOG_EXPORT std::tuple<PipelineStateBuilder, std::unique_ptr<Error>>
createBasicEffect(
    const std::shared_ptr<vfs::FileSystemContext>& fs,
    const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice,
    BasicEffectVariant variant);

} // namespace pomdog::BasicEffect
