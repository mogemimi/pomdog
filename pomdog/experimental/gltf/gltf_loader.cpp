// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/gltf/gltf_loader.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/content/utility/binary_reader.h"
#include "pomdog/content/utility/make_fourcc.h"
#include "pomdog/experimental/gltf/gltf.h"
#include "pomdog/filesystem/file_system.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wzero-as-null-pointer-constant"
#endif
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wuseless-cast"
#pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
#endif
#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable : 4514 5054)
#endif
#include <rapidjson/document.h>
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic pop
#endif
#if defined(_MSC_VER)
#pragma warning(pop)
#endif
#include <algorithm>
#include <fstream>
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::GLTF {
namespace {

namespace BinaryReader = detail::BinaryReader;
using detail::makeFourCC;

[[nodiscard]] std::optional<ComponentType>
ToComponentType(std::uint32_t v) noexcept
{
    switch (v) {
    case 5120:
        return ComponentType::Byte;
    case 5121:
        return ComponentType::Ubyte;
    case 5122:
        return ComponentType::Short;
    case 5123:
        return ComponentType::Ushort;
    case 5125:
        return ComponentType::Uint;
    case 5126:
        return ComponentType::Float;
    default:
        break;
    }
    return std::nullopt;
}

[[nodiscard]] std::optional<AccessorType>
ToAccessorType(std::string_view s) noexcept
{
    if (s == "SCALAR") {
        return AccessorType::Scalar;
    }
    if (s == "VEC2") {
        return AccessorType::Vec2;
    }
    if (s == "VEC3") {
        return AccessorType::Vec3;
    }
    if (s == "VEC4") {
        return AccessorType::Vec4;
    }
    if (s == "MAT2") {
        return AccessorType::Mat2;
    }
    if (s == "MAT3") {
        return AccessorType::Mat3;
    }
    if (s == "MAT4") {
        return AccessorType::Mat4;
    }

    return std::nullopt;
}

[[nodiscard]] std::unique_ptr<Error>
ParseAsset(const rapidjson::Value& asset, GLTF::Asset& result) noexcept
{
    if (auto iter = asset.FindMember("version"); iter == asset.MemberEnd()) {
        return errors::make("cannot find version");
    }
    else if (auto& value = iter->value; !value.IsString()) {
        return errors::make("version must be == string");
    }
    else {
        result.Version = value.GetString();
    }

    if (auto iter = asset.FindMember("generator"); iter == asset.MemberEnd()) {
        return errors::make("cannot find generator");
    }
    else if (auto& value = iter->value; !value.IsString()) {
        return errors::make("version must be == string");
    }
    else {
        result.Generator = value.GetString();
    }

    return nullptr;
}

[[nodiscard]] std::unique_ptr<Error>
ParseUintArray(const rapidjson::Value& array, std::vector<std::uint32_t>& result) noexcept
{
    if (!array.IsArray()) {
        return errors::make("array must be == array");
    }

    result.reserve(array.GetArray().Size());

    for (auto& v : array.GetArray()) {
        if (!v.IsUint()) {
            return errors::make("v must be == uint");
        }
        result.push_back(v.GetUint());
    }

    return nullptr;
}

[[nodiscard]] std::unique_ptr<Error>
ParseQuaternion(const rapidjson::Value& array, Quaternion& result) noexcept
{
    if (!array.IsArray()) {
        return errors::make("array must be == array");
    }

    const auto vec = array.GetArray();
    if (vec.Size() != 4) {
        return errors::make("vec.Size() must be == 4");
    }
    for (auto& v : vec) {
        if (!v.IsFloat()) {
            return errors::make("v must be float type");
        }
    }
    result.x = vec[0].IsFloat();
    result.y = vec[1].IsFloat();
    result.z = vec[2].IsFloat();
    result.w = vec[3].IsFloat();

    return nullptr;
}

[[nodiscard]] std::unique_ptr<Error>
ParseVector3(const rapidjson::Value& array, Vector3& result) noexcept
{
    if (!array.IsArray()) {
        return errors::make("array must be == array");
    }

    const auto vec = array.GetArray();
    if (vec.Size() != 3) {
        return errors::make("vec.Size() must be == 3");
    }
    for (auto& v : vec) {
        if (!v.IsFloat()) {
            return errors::make("v must be float type");
        }
    }
    result.x = vec[0].IsFloat();
    result.y = vec[1].IsFloat();
    result.z = vec[2].IsFloat();

    return nullptr;
}

[[nodiscard]] std::unique_ptr<Error>
ParseScene(const rapidjson::Value& scene, GLTF::Scene& result) noexcept
{
    if (!scene.IsObject()) {
        return errors::make("scene must be == object");
    }

    auto sceneObject = scene.GetObject();

    if (auto iter = sceneObject.FindMember("name"); iter == sceneObject.MemberEnd()) {
        // NOTE: nothing to do.
    }
    else if (auto& value = iter->value; !value.IsString()) {
        return errors::make("name must be == string");
    }
    else {
        result.Name = value.GetString();
    }

    if (auto iter = sceneObject.FindMember("nodes"); iter == sceneObject.MemberEnd()) {
        // NOTE: nothing to do.
    }
    else if (auto err = ParseUintArray(iter->value, result.Nodes); err != nullptr) {
        return errors::wrap(std::move(err), "ParseUintArray() failed");
    }

    return nullptr;
}

[[nodiscard]] std::unique_ptr<Error>
ParseNode(const rapidjson::Value& node, GLTF::Node& result) noexcept
{
    if (!node.IsObject()) {
        return errors::make("node must be == object");
    }

    auto nodeObject = node.GetObject();

    if (auto iter = nodeObject.FindMember("name"); iter == nodeObject.MemberEnd()) {
        // NOTE: nothing to do.
    }
    else if (auto& value = iter->value; !value.IsString()) {
        return errors::make("name must be == string");
    }
    else {
        result.Name = value.GetString();
    }

    if (auto iter = nodeObject.FindMember("mesh"); iter == nodeObject.MemberEnd()) {
        // NOTE: nothing to do.
    }
    else if (auto& value = iter->value; !value.IsUint()) {
        return errors::make("mesh must be == string");
    }
    else {
        result.Mesh = value.GetUint();
    }

    if (auto iter = nodeObject.FindMember("rotation"); iter == nodeObject.MemberEnd()) {
        // NOTE: nothing to do.
    }
    else {
        Quaternion quaternion;
        if (auto err = ParseQuaternion(iter->value, quaternion); err != nullptr) {
            return errors::wrap(std::move(err), "ParseQuaternion() failed");
        }
        result.Rotation = quaternion;
    }

    if (auto iter = nodeObject.FindMember("translation"); iter == nodeObject.MemberEnd()) {
        // NOTE: nothing to do.
    }
    else {
        Vector3 vec;
        if (auto err = ParseVector3(iter->value, vec); err != nullptr) {
            return errors::wrap(std::move(err), "ParseVector3() failed");
        }
        result.Translation = vec;
    }

    if (auto iter = nodeObject.FindMember("scale"); iter == nodeObject.MemberEnd()) {
        // NOTE: nothing to do.
    }
    else {
        Vector3 vec;
        if (auto err = ParseVector3(iter->value, vec); err != nullptr) {
            return errors::wrap(std::move(err), "ParseVector3() failed");
        }
        result.Scale = vec;
    }

    return nullptr;
}

[[nodiscard]] std::unique_ptr<Error>
ParseMaterial(const rapidjson::Value& material, GLTF::Material& result) noexcept
{
    if (!material.IsObject()) {
        return errors::make("material must be == object");
    }

    result.DoubleSided = false;

    auto materialObject = material.GetObject();

    if (auto iter = materialObject.FindMember("name"); iter == materialObject.MemberEnd()) {
        // NOTE: nothing to do.
    }
    else if (auto& value = iter->value; !value.IsString()) {
        return errors::make("name must be == string");
    }
    else {
        result.Name = value.GetString();
    }

    if (auto iter = materialObject.FindMember("doubleSided"); iter == materialObject.MemberEnd()) {
        // NOTE: nothing to do.
    }
    else if (auto& value = iter->value; !value.IsBool()) {
        return errors::make("doubleSided must be == bool");
    }
    else {
        result.DoubleSided = value.GetBool();
    }

    return nullptr;
}

[[nodiscard]] std::unique_ptr<Error>
ParseAttributeList(const rapidjson::Value& attributes, GLTF::AttributeList& result) noexcept
{
    if (!attributes.IsObject()) {
        return errors::make("attributes must be == object");
    }

    auto attributesObject = attributes.GetObject();

    for (auto& kv : attributesObject) {
        if (!kv.name.IsString()) {
            return errors::make("kv.name must be == string");
        }
        if (!kv.value.IsUint()) {
            return errors::make("kv.value must be == uint");
        }
        result.emplace(kv.name.GetString(), kv.value.GetUint());
    }

    return nullptr;
}

[[nodiscard]] std::unique_ptr<Error>
ParsePrimitive(const rapidjson::Value& primitive, GLTF::Primitive& result) noexcept
{
    if (!primitive.IsObject()) {
        return errors::make("primitive must be == object");
    }

    auto primitiveObject = primitive.GetObject();

    if (auto iter = primitiveObject.FindMember("attributes"); iter == primitiveObject.MemberEnd()) {
        // NOTE: nothing to do.
    }
    else if (auto err = ParseAttributeList(iter->value, result.Attributes); err != nullptr) {
        return errors::wrap(std::move(err), "ParseAttributeList() failed");
    }

    if (auto iter = primitiveObject.FindMember("indices"); iter == primitiveObject.MemberEnd()) {
        // NOTE: nothing to do.
    }
    else if (auto& value = iter->value; !value.IsUint()) {
        return errors::make("indices must be == uint");
    }
    else {
        result.Indices = value.GetUint();
    }

    if (auto iter = primitiveObject.FindMember("material"); iter == primitiveObject.MemberEnd()) {
        // NOTE: nothing to do.
    }
    else if (auto& value = iter->value; !value.IsUint()) {
        return errors::make("material must be == uint");
    }
    else {
        result.Material = value.GetUint();
    }

    return nullptr;
}

[[nodiscard]] std::unique_ptr<Error>
ParseMesh(const rapidjson::Value& mesh, GLTF::Mesh& result) noexcept
{
    if (!mesh.IsObject()) {
        return errors::make("mesh must be == object");
    }

    auto meshObject = mesh.GetObject();

    if (auto iter = meshObject.FindMember("name"); iter == meshObject.MemberEnd()) {
        // NOTE: nothing to do.
    }
    else if (auto& value = iter->value; !value.IsString()) {
        return errors::make("name must be == string");
    }
    else {
        result.Name = value.GetString();
    }

    if (auto iter = meshObject.FindMember("primitives"); iter == meshObject.MemberEnd()) {
        // NOTE: nothing to do.
    }
    else if (auto& value = iter->value; !value.IsArray()) {
        return errors::make("primitives must be == object");
    }
    else {
        for (auto& primitive : value.GetArray()) {
            GLTF::Primitive gltfPrimitive;
            if (auto err = ParsePrimitive(primitive, gltfPrimitive); err != nullptr) {
                return errors::wrap(std::move(err), "ParsePrimitive() failed");
            }
            result.Primitives.push_back(std::move(gltfPrimitive));
        }
    }

    return nullptr;
}

[[nodiscard]] std::unique_ptr<Error>
ParseAccessor(const rapidjson::Value& accessor, GLTF::Accessor& result) noexcept
{
    if (!accessor.IsObject()) {
        return errors::make("accessor must be == object");
    }

    result.BufferView = 0;
    result.Count = 0;

    auto accessorObject = accessor.GetObject();

    if (auto iter = accessorObject.FindMember("bufferView"); iter == accessorObject.MemberEnd()) {
        // NOTE: nothing to do.
    }
    else if (auto& value = iter->value; !value.IsUint()) {
        return errors::make("bufferView must be == uint");
    }
    else {
        result.BufferView = value.GetUint();
    }

    if (auto iter = accessorObject.FindMember("count"); iter == accessorObject.MemberEnd()) {
        // NOTE: nothing to do.
    }
    else if (auto& value = iter->value; !value.IsUint()) {
        return errors::make("count must be == uint");
    }
    else {
        result.Count = value.GetUint();
    }

    if (auto iter = accessorObject.FindMember("componentType"); iter == accessorObject.MemberEnd()) {
        // NOTE: nothing to do.
    }
    else if (auto& value = iter->value; !value.IsUint()) {
        return errors::make("componentType must be == uint");
    }
    else if (auto v = ToComponentType(value.GetUint()); v == std::nullopt) {
        return errors::make("invalid componentType");
    }
    else {
        result.ComponentType = *v;
    }

    if (auto iter = accessorObject.FindMember("type"); iter == accessorObject.MemberEnd()) {
        // NOTE: nothing to do.
    }
    else if (auto& value = iter->value; !value.IsString()) {
        return errors::make("type must be == string");
    }
    else if (auto v = ToAccessorType(value.GetString()); v == std::nullopt) {
        return errors::make("ToAccessorType() failed");
    }
    else {
        result.Type = *v;
    }

    return nullptr;
}

[[nodiscard]] std::unique_ptr<Error>
ParseBufferView(const rapidjson::Value& bufferView, GLTF::BufferView& result) noexcept
{
    if (!bufferView.IsObject()) {
        return errors::make("bufferView must be == object");
    }

    result.Buffer = 0;
    result.ByteOffset = 0;
    result.ByteLength = 0;
    result.ByteStride = 0;
    result.Target = GLTF::Target::ArrayBuffer;

    auto bufferViewObject = bufferView.GetObject();

    if (auto iter = bufferViewObject.FindMember("buffer"); iter == bufferViewObject.MemberEnd()) {
        // NOTE: nothing to do.
    }
    else if (auto& value = iter->value; !value.IsUint()) {
        return errors::make("buffer must be == uint");
    }
    else {
        result.Buffer = value.GetUint();
    }

    if (auto iter = bufferViewObject.FindMember("byteLength"); iter == bufferViewObject.MemberEnd()) {
        // NOTE: nothing to do.
    }
    else if (auto& value = iter->value; !value.IsUint()) {
        return errors::make("byteLength must be == uint");
    }
    else {
        result.ByteLength = value.GetUint();
    }

    if (auto iter = bufferViewObject.FindMember("byteOffset"); iter == bufferViewObject.MemberEnd()) {
        // NOTE: nothing to do.
    }
    else if (auto& value = iter->value; !value.IsUint()) {
        return errors::make("byteOffset must be == uint");
    }
    else {
        result.ByteOffset = value.GetUint();
    }

    if (auto iter = bufferViewObject.FindMember("byteStride"); iter == bufferViewObject.MemberEnd()) {
        // NOTE: nothing to do.
    }
    else if (auto& value = iter->value; !value.IsUint()) {
        return errors::make("byteStride must be == uint");
    }
    else {
        result.ByteStride = value.GetUint();
    }

    return nullptr;
}

[[nodiscard]] std::unique_ptr<Error>
ParseBuffer(const rapidjson::Value& buffer, GLTF::Buffer& result) noexcept
{
    if (!buffer.IsObject()) {
        return errors::make("buffer must be == object");
    }

    result.ByteLength = 0;

    auto bufferObject = buffer.GetObject();
    if (auto iter = bufferObject.FindMember("name"); iter == bufferObject.MemberEnd()) {
        // NOTE: nothing to do.
    }
    else if (auto& value = iter->value; !value.IsString()) {
        return errors::make("name must be == string");
    }
    else {
        result.Name = value.GetString();
    }

    if (auto iter = bufferObject.FindMember("uri"); iter == bufferObject.MemberEnd()) {
        // NOTE: nothing to do.
    }
    else if (auto& value = iter->value; !value.IsString()) {
        return errors::make("uri must be == string");
    }
    else {
        result.URI = value.GetString();
    }

    if (auto iter = bufferObject.FindMember("byteLength"); iter == bufferObject.MemberEnd()) {
        // NOTE: nothing to do.
    }
    else if (auto& value = iter->value; !value.IsUint()) {
        return errors::make("byteLength must be == uint");
    }
    else {
        result.ByteLength = value.GetUint();
    }

    return nullptr;
}

[[nodiscard]] std::unique_ptr<Error>
ParseJSON(const char* json, std::size_t jsonLength, GLTF::Document& result) noexcept
{
    rapidjson::Document doc;
    doc.Parse(json, jsonLength);

    if (doc.HasParseError()) {
        return errors::make("failed to parse JSON");
    }
    if (!doc.IsObject() || doc.MemberBegin() == doc.MemberEnd()) {
        return errors::make("doc must be object type");
    }

    if (auto iter = doc.FindMember("asset"); iter == doc.MemberEnd()) {
        return errors::make("cannot find asset object");
    }
    else if (auto& asset = iter->value; !asset.IsObject()) {
        return errors::make("asset must be == object");
    }
    else {
        if (auto err = ParseAsset(asset, result.Asset); err != nullptr) {
            return errors::wrap(std::move(err), "ParseAsset() failed");
        }
    }

    if (auto iter = doc.FindMember("scene"); iter == doc.MemberEnd()) {
        // NOTE: nothing to do
    }
    else if (auto& scene = iter->value; !scene.IsUint()) {
        return errors::make("scene must be == object");
    }
    else {
        result.Scene = scene.GetUint();
    }

    if (auto iter = doc.FindMember("scenes"); iter == doc.MemberEnd()) {
        // NOTE: nothing to do
    }
    else if (auto& scenes = iter->value; !scenes.IsArray()) {
        return errors::make("scenes must be == array");
    }
    else {
        for (auto& scene : scenes.GetArray()) {
            GLTF::Scene gltfScene;
            if (auto err = ParseScene(scene, gltfScene); err != nullptr) {
                return errors::wrap(std::move(err), "ParseScene() failed");
            }
            result.Scenes.push_back(std::move(gltfScene));
        }
    }

    if (auto iter = doc.FindMember("nodes"); iter == doc.MemberEnd()) {
        // NOTE: nothing to do
    }
    else if (auto& nodes = iter->value; !nodes.IsArray()) {
        return errors::make("nodes must be == array");
    }
    else {
        for (auto& node : nodes.GetArray()) {
            GLTF::Node gltfNode;
            if (auto err = ParseNode(node, gltfNode); err != nullptr) {
                return errors::wrap(std::move(err), "ParseNode() failed");
            }
            result.Nodes.push_back(std::move(gltfNode));
        }
    }

    if (auto iter = doc.FindMember("materials"); iter == doc.MemberEnd()) {
        // NOTE: nothing to do
    }
    else if (auto& materials = iter->value; !materials.IsArray()) {
        return errors::make("materials must be == array");
    }
    else {
        for (auto& material : materials.GetArray()) {
            GLTF::Material gltfMaterial;
            if (auto err = ParseMaterial(material, gltfMaterial); err != nullptr) {
                return errors::wrap(std::move(err), "ParseMaterial() failed");
            }
            result.Materials.push_back(std::move(gltfMaterial));
        }
    }

    if (auto iter = doc.FindMember("meshes"); iter == doc.MemberEnd()) {
        // NOTE: nothing to do
    }
    else if (auto& meshes = iter->value; !meshes.IsArray()) {
        return errors::make("meshes must be == array");
    }
    else {
        for (auto& mesh : meshes.GetArray()) {
            GLTF::Mesh gltfMesh;
            if (auto err = ParseMesh(mesh, gltfMesh); err != nullptr) {
                return errors::wrap(std::move(err), "ParseMesh() failed");
            }
            result.Meshes.push_back(std::move(gltfMesh));
        }
    }

    if (auto iter = doc.FindMember("accessors"); iter == doc.MemberEnd()) {
        // NOTE: nothing to do
    }
    else if (auto& accessors = iter->value; !accessors.IsArray()) {
        return errors::make("accessors must be == array");
    }
    else {
        for (auto& accessor : accessors.GetArray()) {
            GLTF::Accessor gltfAccessor;
            if (auto err = ParseAccessor(accessor, gltfAccessor); err != nullptr) {
                return errors::wrap(std::move(err), "ParseAccessor() failed");
            }
            result.Accessors.push_back(std::move(gltfAccessor));
        }
    }

    if (auto iter = doc.FindMember("bufferViews"); iter == doc.MemberEnd()) {
        // NOTE: nothing to do
    }
    else if (auto& bufferViews = iter->value; !bufferViews.IsArray()) {
        return errors::make("bufferViews must be == array");
    }
    else {
        for (auto& bufferView : bufferViews.GetArray()) {
            GLTF::BufferView gltfBufferView;
            if (auto err = ParseBufferView(bufferView, gltfBufferView); err != nullptr) {
                return errors::wrap(std::move(err), "ParseBufferView() failed");
            }
            result.BufferViews.push_back(std::move(gltfBufferView));
        }
    }

    if (auto iter = doc.FindMember("buffers"); iter == doc.MemberEnd()) {
        // NOTE: nothing to do
    }
    else if (auto& buffers = iter->value; !buffers.IsArray()) {
        return errors::make("buffers must be == array");
    }
    else {
        for (auto& buffer : buffers.GetArray()) {
            GLTF::Buffer gltfBuffer;
            if (auto err = ParseBuffer(buffer, gltfBuffer); err != nullptr) {
                return errors::wrap(std::move(err), "ParseBuffer() failed");
            }
            result.Buffers.push_back(std::move(gltfBuffer));
        }
    }

    return nullptr;
}

} // namespace

[[nodiscard]] std::tuple<std::shared_ptr<Document>, std::unique_ptr<Error>>
Open(const std::string& filePath) noexcept
{
    auto [fileSize, sizeErr] = FileSystem::getFileSize(filePath);
    if (sizeErr != nullptr) {
        auto err = errors::wrap(std::move(sizeErr), "failed to get file size, " + filePath);
        return std::make_tuple(nullptr, std::move(err));
    }

    if (fileSize <= 12) {
        auto err = errors::make("the font file is too small, " + filePath);
        return std::make_tuple(nullptr, std::move(err));
    }

    std::ifstream stream{filePath, std::ifstream::binary};
    std::size_t readByteSize = 0;

    if (!stream) {
        auto err = errors::make("cannot open the file, " + filePath);
        return std::make_tuple(nullptr, std::move(err));
    }

    constexpr auto magicGLB = makeFourCC('g', 'l', 'T', 'F');
    static_assert(magicGLB == 0x46546C67);

    if (auto magic = BinaryReader::read<std::uint32_t>(stream); magic != magicGLB) {
        auto err = errors::make("invalid Binary glTF format, " + filePath);
        return std::make_tuple(nullptr, std::move(err));
    }

    if (auto version = BinaryReader::read<std::uint32_t>(stream); version != 2) {
        auto err = errors::make("version does not much, " + filePath);
        return std::make_tuple(nullptr, std::move(err));
    }

    const auto totalLength = BinaryReader::read<std::uint32_t>(stream);
    if (totalLength > fileSize) {
        auto err = errors::make("length must be <= fileSize, " + filePath);
        return std::make_tuple(nullptr, std::move(err));
    }
    readByteSize += 12;

    if (!stream) {
        auto err = errors::make("failed to read file, " + filePath);
        return std::make_tuple(nullptr, std::move(err));
    }

    std::size_t bufferCount = 0;
    auto doc = std::make_shared<Document>();

    while (stream) {
        const auto chunkLength = BinaryReader::read<std::uint32_t>(stream);
        if (!stream) {
            auto err = errors::make("failed to read chuk length, " + filePath);
            return std::make_tuple(nullptr, std::move(err));
        }
        if (chunkLength <= 0) {
            auto err = errors::make("chunkLength must be > 0, " + filePath);
            return std::make_tuple(nullptr, std::move(err));
        }
        readByteSize += 4;

        const auto chunkType = BinaryReader::read<std::uint32_t>(stream);
        if (!stream) {
            auto err = errors::make("failed to read chuk type, " + filePath);
            return std::make_tuple(nullptr, std::move(err));
        }
        readByteSize += 4;

        auto chunkData = BinaryReader::readArray<std::uint8_t>(stream, chunkLength);
        if (!stream) {
            auto err = errors::make("failed to read chunk data, " + filePath);
            return std::make_tuple(nullptr, std::move(err));
        }
        POMDOG_ASSERT(chunkLength == chunkData.size());
        readByteSize += chunkData.size();

        constexpr auto chunkTypeJSON = makeFourCC('J', 'S', 'O', 'N');
        constexpr auto chunkTypeBIN = makeFourCC('B', 'I', 'N', '\0');

        static_assert(chunkTypeJSON == 0x4E4F534A);
        static_assert(chunkTypeBIN == 0x004E4942);

        if (chunkType == chunkTypeJSON) {
            if (auto err = ParseJSON(reinterpret_cast<const char*>(chunkData.data()), chunkData.size(), *doc); err != nullptr) {
                auto wrapped = errors::wrap(std::move(err), "ParseJSON failed, " + filePath);
                return std::make_tuple(nullptr, std::move(wrapped));
            }
        }
        else if (chunkType == chunkTypeBIN) {
            if (bufferCount >= doc->Buffers.size()) {
                auto err = errors::make("invalid buffer chunk, " + filePath);
                return std::make_tuple(nullptr, std::move(err));
            }

            auto& buffer = doc->Buffers[bufferCount];
            buffer.Data = std::move(chunkData);

            if (buffer.ByteLength > buffer.Data.size()) {
                auto err = errors::make("buffer.ByteLength must be <= buffer.Data.size(), " + filePath);
                return std::make_tuple(nullptr, std::move(err));
            }

            bufferCount += 1;
        }
        else {
            auto err = errors::make("invalid chunk type, " + filePath);
            return std::make_tuple(nullptr, std::move(err));
        }

        if (readByteSize >= totalLength) {
            break;
        }
    }

    return std::make_tuple(std::move(doc), nullptr);
}
} // namespace pomdog::GLTF
