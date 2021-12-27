// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/math/matrix2x2.h"
#include "pomdog/math/matrix3x3.h"
#include "pomdog/math/matrix4x4.h"
#include "pomdog/math/quaternion.h"
#include "pomdog/math/vector2.h"
#include "pomdog/math/vector3.h"
#include "pomdog/math/vector4.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::GLTF {

class Accessor;
class Asset;
class Buffer;
class BufferView;
class Material;
class Mesh;
class Node;
class Scene;
class Texture;

class Asset final {
public:
    std::string Version;
    std::string Generator;
};

class Scene final {
public:
    std::string Name;
    std::vector<std::uint32_t> Nodes;
};

class Node final {
public:
    std::string Name;
    std::optional<std::uint32_t> Camera;
    std::vector<std::uint32_t> Children;
    std::optional<std::uint32_t> Skin;
    std::optional<Matrix4x4> Matrix;
    std::optional<std::uint32_t> Mesh;
    std::optional<Quaternion> Rotation;
    std::optional<Vector3> Scale;
    std::optional<Vector3> Translation;
};

class TextureInfo final {
public:
    std::uint32_t Index;
    std::uint32_t TexCoord;
};

class PBRMetallicRoughness final {
public:
    std::optional<Vector4> BaseColorFactor;
    std::optional<float> MetallicFactor;
    std::optional<float> RoughnessFactor;
    std::optional<TextureInfo> BaseColorTexture;
    std::optional<TextureInfo> MetallicRoughnessTexture;
};

class NormalTexture final {
public:
    std::optional<std::uint32_t> Index;
    std::uint32_t TexCoord;
    std::optional<float> Scale;
};

class OcclusionTexture final {
public:
    std::optional<std::uint32_t> Index;
    std::uint32_t TexCoord;
    std::optional<float> Strength;
};

class Material final {
public:
    std::string Name;
    std::optional<GLTF::PBRMetallicRoughness> PBRMetallicRoughness;
    std::optional<GLTF::NormalTexture> NormalTexture;
    std::optional<GLTF::OcclusionTexture> OcclusionTexture;
    std::optional<TextureInfo> EmissiveTexture;
    std::optional<Vector3> EmissiveFactor;
    std::optional<float> AlphaCutoff;
    bool DoubleSided = false;
};

using AttributeList = std::unordered_map<std::string, std::uint32_t>;

enum class PrimitiveMode : std::uint8_t {
    Triangles,
    Points,
    Lines,
    LineLoop,
    LineStrip,
    TriangleStrip,
    TriangleFan,
};

class Primitive final {
public:
    AttributeList Attributes;
    std::optional<std::uint32_t> Indices;
    std::optional<std::uint32_t> Material;
    PrimitiveMode Mode;
    std::vector<AttributeList> Targets;
};

class Mesh final {
public:
    std::string Name;
    std::vector<Primitive> Primitives;
    std::vector<float> Weights;
};

enum class ComponentType : std::uint8_t {
    Byte,
    Ubyte,
    Short,
    Ushort,
    Uint,
    Float,
};

enum class AccessorType : std::uint8_t {
    Scalar,
    Vec2,
    Vec3,
    Vec4,
    Mat2,
    Mat3,
    Mat4,
};

class SparseIndices final {
public:
    std::uint32_t BufferView;
    std::uint32_t ByteOffset;
    GLTF::ComponentType ComponentType;
};

class SparseValues final {
public:
    std::uint32_t BufferView;
    std::uint32_t ByteOffset;
};

class Sparse final {
public:
    std::uint32_t Count;
    GLTF::SparseIndices SparseIndices;
    GLTF::SparseValues SparseValues;
};

using AccessorValue = std::variant<std::monostate, float, Vector2, Vector3, Vector4, Matrix2x2, Matrix3x3, Matrix4x4>;

class Accessor final {
public:
    std::string Name;
    std::optional<GLTF::Sparse> Sparse;
    std::optional<std::uint32_t> BufferView;
    std::uint32_t ByteOffset = 0;
    std::uint32_t Count = 0;
    AccessorValue Min;
    AccessorValue Max;
    GLTF::ComponentType ComponentType;
    GLTF::AccessorType Type;
    bool Normalized = false;
};

enum class Target : std::uint16_t {
    ArrayBuffer = 34962,
    ElementArrayBuffer = 34963,
};

class BufferView final {
public:
    std::uint32_t Buffer;
    std::uint32_t ByteOffset;
    std::uint32_t ByteLength;
    std::uint32_t ByteStride;
    GLTF::Target Target;
};

class Buffer final {
public:
    std::string Name;
    std::string URI;
    std::uint32_t ByteLength;
    std::vector<std::uint8_t> Data;
};

class Document final {
public:
    GLTF::Asset Asset;
    std::vector<GLTF::Accessor> Accessors;
    std::vector<GLTF::Buffer> Buffers;
    std::vector<GLTF::BufferView> BufferViews;
    std::vector<GLTF::Material> Materials;
    std::vector<GLTF::Mesh> Meshes;
    std::vector<GLTF::Node> Nodes;
    std::optional<std::uint32_t> Scene;
    std::vector<GLTF::Scene> Scenes;
};

} // namespace pomdog::GLTF
