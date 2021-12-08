// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/spine/skeleton_desc_loader.hpp"
#include "pomdog/application/file_system.hpp"
#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/content/asset_manager.hpp"
#include "pomdog/content/utility/binary_reader.hpp"
#include "pomdog/math/degree.hpp"
#include "pomdog/math/math.hpp"
#include "pomdog/utility/assert.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wzero-as-null-pointer-constant"
#endif
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic push
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
#include <type_traits>
#include <utility>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::spine {
namespace {

using skeletal2d::AnimationTimeInterval;
using skeletal2d::JointIndex;

template <typename T>
JointIndex ToJointIndex(T index)
{
    static_assert(std::is_integral_v<T>);
    static_assert(!std::is_floating_point_v<T>);
    static_assert(!std::is_pointer_v<T>);
    static_assert(std::is_same_v<decltype(std::declval<JointIndex>().Data), std::uint8_t>);

    POMDOG_ASSERT(index >= 0);
    POMDOG_ASSERT(index <= std::numeric_limits<std::uint8_t>::max());
    return static_cast<std::uint8_t>(index);
}

template <typename T>
std::uint16_t ToUInt16(T n)
{
    static_assert(std::is_unsigned_v<T>);
    POMDOG_ASSERT(n <= std::numeric_limits<std::uint16_t>::max());
    return static_cast<std::uint16_t>(n);
}

void ReadJsonMember(const rapidjson::Value& object, const char* memberName, std::string& output)
{
    if (!object.HasMember(memberName)) {
        return;
    }

    auto& memberObject = object[memberName];
    if (memberObject.IsString()) {
        output.assign(memberObject.GetString(), memberObject.GetStringLength());
    }
}

void ReadJsonMember(const rapidjson::Value& object, const char* memberName, float& output)
{
    if (!object.HasMember(memberName)) {
        return;
    }

    auto& memberObject = object[memberName];
    if (memberObject.IsNumber()) {
        output = memberObject.GetFloat();
    }
}

void ReadJsonMember(const rapidjson::Value& object, const char* memberName, AnimationTimeInterval& output)
{
    if (!object.HasMember(memberName)) {
        return;
    }

    auto& memberObject = object[memberName];
    if (memberObject.IsNumber()) {
        output = AnimationTimeInterval(memberObject.GetFloat());
    }
}

void ReadJsonMember(const rapidjson::Value& object, const char* memberName, Radian<float>& output)
{
    if (!object.HasMember(memberName)) {
        return;
    }

    auto& memberObject = object[memberName];
    if (memberObject.IsNumber()) {
        Degree<float> degreeAngle = memberObject.GetFloat();
        while (degreeAngle > 180.0f) {
            degreeAngle -= 360.0f;
        }
        while (degreeAngle < -180.0f) {
            degreeAngle += 360.0f;
        }
        output = math::ToRadians(degreeAngle);
    }
}

void ReadJsonMember(const rapidjson::Value& object, const char* memberName, Degree<float>& output)
{
    if (!object.HasMember(memberName)) {
        return;
    }

    auto& memberObject = object[memberName];
    if (memberObject.IsNumber()) {
        output.value = memberObject.GetFloat();
    }
}

template <typename T, T Denom>
void ReadJsonMember(
    const rapidjson::Value& object,
    const char* memberName,
    skeletal2d::detail::CompressedFloat<T, Denom>& output)
{
    if (!object.HasMember(memberName)) {
        return;
    }

    auto& memberObject = object[memberName];
    if (memberObject.IsNumber()) {
        output = memberObject.GetFloat();
    }
}

void ReadJsonMember(const rapidjson::Value& object, const char* memberName, KeyframeCurve& output)
{
    output = KeyframeCurve::Liener;
    if (!object.HasMember(memberName)) {
        return;
    }

    auto& memberObject = object[memberName];
    if (memberObject.IsString()) {
        if (memberObject.GetString() != std::string("linear")) {
            output = KeyframeCurve::Stepped;
        }
    }
    else if (memberObject.IsArray()) {
        ///@todo Not implemented
        output = KeyframeCurve::Bezier;
    }
}

void ReadJsonMember(const rapidjson::Value& object, const char* memberName, std::uint16_t& output)
{
    if (!object.HasMember(memberName)) {
        return;
    }

    auto& memberObject = object[memberName];
    if (memberObject.IsUint()) {
        output = static_cast<std::uint16_t>(memberObject.GetUint());
    }
}

void ReadJsonMember(const rapidjson::Value& object, const char* memberName, Color& output)
{
    if (!object.HasMember(memberName)) {
        return;
    }

    auto& memberObject = object[memberName];
    if (memberObject.IsString()) {
        auto hexColor = static_cast<std::uint32_t>(
            std::stoul(memberObject.GetString(), nullptr, 16));
        output = Color::FromPackedValue(hexColor);
    }
}

std::vector<BoneDesc> ReadBones(const rapidjson::Value& bonesDOM)
{
    if (!bonesDOM.IsArray() || bonesDOM.Empty()) {
        ///@todo Not implemented
        // Error
        return {};
    }

    std::vector<BoneDesc> bones;
    bones.reserve(bonesDOM.Size());

    for (rapidjson::SizeType index = 0; index < bonesDOM.Size(); ++index) {
        auto& boneDOM = bonesDOM[index];

        if (!boneDOM.IsObject()) {
            ///@todo Not implemented
            // Error
            continue;
        }

        if (!boneDOM.HasMember("name") || !boneDOM["name"].IsString()) {
            ///@todo Not implemented
            // Error
            continue;
        }

        BoneDesc boneDesc;

        boneDesc.Name = boneDOM["name"].GetString();
        boneDesc.Pose.Translate.X = 0;
        boneDesc.Pose.Translate.Y = 0;
        boneDesc.Pose.Scale = 1;
        boneDesc.Pose.Rotation = 0;

        ReadJsonMember(boneDOM, "parent", boneDesc.Parent);
        //ReadJsonMember(boneDOM, "length", boneDesc.Length);
        ReadJsonMember(boneDOM, "x", boneDesc.Pose.Translate.X);
        ReadJsonMember(boneDOM, "y", boneDesc.Pose.Translate.Y);
        ReadJsonMember(boneDOM, "scaleY", boneDesc.Pose.Scale);
        ReadJsonMember(boneDOM, "scaleX", boneDesc.Pose.Scale);

        Radian<float> rotation = 0;
        ReadJsonMember(boneDOM, "rotation", rotation);
        boneDesc.Pose.Rotation = rotation.value;

        bones.push_back(std::move(boneDesc));
    }
    return bones;
}

JointIndex FindJointIndex(const char* boneName, const std::vector<BoneDesc>& bones)
{
    auto iter = std::find_if(std::begin(bones), std::end(bones), [&boneName](const BoneDesc& boneDesc) {
        return boneDesc.Name == boneName;
    });
    POMDOG_ASSERT(iter != std::end(bones));
    if (iter == std::end(bones)) {
        ///@todo Not implemented
        // Error: Cannot find bone
        return {};
    }
    return ToJointIndex(std::distance(std::begin(bones), iter));
}

std::vector<SlotDesc> ReadSlots(const rapidjson::Value& slotsDOM, const std::vector<BoneDesc>& bones)
{
    if (!slotsDOM.IsArray()) {
        ///@todo Not implemented
        // Error
        return {};
    }

    std::vector<SlotDesc> slots;
    slots.reserve(slotsDOM.Size());

    for (rapidjson::SizeType index = 0; index < slotsDOM.Size(); ++index) {
        auto& slot = slotsDOM[index];
        if (!slot.IsObject() || !slot.HasMember("name") || !slot.HasMember("bone") || !slot.HasMember("attachment")) {
            ///@todo Not implemented
            // Error
            continue;
        }
        auto& name = slot["name"];
        auto& bone = slot["bone"];
        auto& attachment = slot["attachment"];

        if (!name.IsString() || !bone.IsString() || !attachment.IsString()) {
            ///@todo Not implemented
            // Error
            continue;
        }

        SlotDesc slotDesc;
        slotDesc.Name = name.GetString();
        slotDesc.Attachement = attachment.GetString();
        slotDesc.Color = Color::White;
        slotDesc.Joint = FindJointIndex(bone.GetString(), bones);

        ReadJsonMember(slot, "color", slotDesc.Color);

        slots.push_back(std::move(slotDesc));
    }

    return slots;
}

std::vector<SkinnedMeshVertexDesc> ReadSkinnedMeshVertices(
    const rapidjson::Value& verticesArray, const rapidjson::Value& uvsArray)
{
    POMDOG_ASSERT(verticesArray.IsArray());
    POMDOG_ASSERT(uvsArray.IsArray());

    auto const verticesCount = uvsArray.Size() / 2;

    std::vector<SkinnedMeshVertexDesc> vertices;
    vertices.reserve(verticesCount);

    std::uint32_t verticesIter = 0;
    auto uvsIter = uvsArray.Begin();

    while (uvsIter != uvsArray.End()) {
        POMDOG_ASSERT(verticesIter < verticesArray.Size());

        struct LocalVertex final {
            Vector2 Position;
            float Weight;
            skeletal2d::JointIndex JointIndex;
        };

        POMDOG_ASSERT(verticesArray[verticesIter].IsUint());
        const auto sourceBoneCount = [](std::uint32_t x) -> std::uint8_t {
            static_assert(std::is_unsigned_v<decltype(x)>, "x >= 0");
            POMDOG_ASSERT(x <= std::numeric_limits<std::uint8_t>::max());
            return static_cast<std::uint8_t>(x);
        }(verticesArray[verticesIter].GetUint());
        ++verticesIter;
        POMDOG_ASSERT(sourceBoneCount > 0);

        std::vector<LocalVertex> localVertices;
        localVertices.reserve(sourceBoneCount);

        for (std::uint8_t index = 0; index < sourceBoneCount; ++index) {
            POMDOG_ASSERT(verticesIter < verticesArray.Size());

            LocalVertex vertex;

            POMDOG_ASSERT(verticesArray[verticesIter].IsUint());
            vertex.JointIndex = ToJointIndex(verticesArray[verticesIter].GetUint());
            ++verticesIter;

            POMDOG_ASSERT(verticesArray[verticesIter].IsNumber());
            vertex.Position.X = verticesArray[verticesIter].GetFloat();
            ++verticesIter;

            POMDOG_ASSERT(verticesArray[verticesIter].IsNumber());
            vertex.Position.Y = verticesArray[verticesIter].GetFloat();
            ++verticesIter;

            POMDOG_ASSERT(verticesArray[verticesIter].IsNumber());
            vertex.Weight = verticesArray[verticesIter].GetFloat();
            ++verticesIter;

            localVertices.push_back(std::move(vertex));
        }

        SkinnedMeshVertexDesc vertex;

        std::sort(std::begin(localVertices), std::end(localVertices),
            [](const LocalVertex& a, const LocalVertex& b) {
                POMDOG_ASSERT(a.JointIndex);
                POMDOG_ASSERT(b.JointIndex);
                return a.Weight > b.Weight;
            });

        if (localVertices.size() >= vertex.Joints.size()) {
            float accumulatedWeight = 0;
            for (std::uint8_t index = 0; index < vertex.Joints.size(); ++index) {
                POMDOG_ASSERT(index < vertex.Joints.size());
                accumulatedWeight += localVertices[index].Weight;
            }

            if (accumulatedWeight < 1) {
                auto weightDiff = (1 - accumulatedWeight) / vertex.Joints.size();
                for (auto& localVertex : localVertices) {
                    localVertex.Weight += weightDiff;
                }
            }
        }

        vertex.Position = localVertices.front().Position;

        auto boneCount = std::min(vertex.Joints.size(), localVertices.size());
        for (std::uint8_t index = 0; index < boneCount; ++index) {
            POMDOG_ASSERT(vertex.Joints.size() == 4);
            POMDOG_ASSERT(index < vertex.Joints.size());
            vertex.Joints[index] = localVertices[index].JointIndex;

            POMDOG_ASSERT(vertex.Weights.size() == 3);
            if (index < vertex.Weights.size()) {
                vertex.Weights[index] = localVertices[index].Weight;
            }
        }

        POMDOG_ASSERT(uvsIter->IsNumber());
        vertex.TextureCoordinate.X = uvsIter->GetFloat();
        ++uvsIter;
        POMDOG_ASSERT(uvsIter != uvsArray.End());
        POMDOG_ASSERT(uvsIter->IsNumber());
        vertex.TextureCoordinate.Y = uvsIter->GetFloat();
        ++uvsIter;

        vertices.push_back(std::move(vertex));
    }

    return vertices;
}

std::vector<std::uint16_t> ReadSkinnedMeshIndices(const rapidjson::Value& indicesArray)
{
    POMDOG_ASSERT(indicesArray.IsArray());

    std::vector<std::uint16_t> indices;
    indices.reserve(indicesArray.Size());

    for (auto iter = indicesArray.Begin(); iter != indicesArray.End(); ++iter) {
        POMDOG_ASSERT(iter->IsUint());
        indices.push_back(ToUInt16(iter->GetUint()));
    }

    return indices;
}

AttachmentDesc ReadAttachment(const rapidjson::Value::ConstMemberIterator& iter)
{
    POMDOG_ASSERT(iter->name.IsString());
    POMDOG_ASSERT(iter->value.IsObject());

    AttachmentDesc attachmentDesc;
    attachmentDesc.Name = iter->name.GetString();
    attachmentDesc.Scale = {1, 1};
    attachmentDesc.Translate = {1, 1};
    attachmentDesc.Rotation = 0;
    attachmentDesc.Height = 1;
    attachmentDesc.Width = 1;

    auto& attachmentObject = iter->value;

    ReadJsonMember(attachmentObject, "x", attachmentDesc.Translate.X);
    ReadJsonMember(attachmentObject, "y", attachmentDesc.Translate.Y);
    ReadJsonMember(attachmentObject, "scaleX", attachmentDesc.Scale.X);
    ReadJsonMember(attachmentObject, "scaleY", attachmentDesc.Scale.Y);
    ReadJsonMember(attachmentObject, "rotation", attachmentDesc.Rotation);
    ReadJsonMember(attachmentObject, "width", attachmentDesc.Width);
    ReadJsonMember(attachmentObject, "height", attachmentDesc.Height);

    return attachmentDesc;
}

std::tuple<SkinnedMeshAttachmentDesc, std::unique_ptr<Error>>
ReadSkinnedMeshAttachment(const rapidjson::Value::ConstMemberIterator& iter)
{
    POMDOG_ASSERT(iter->name.IsString());
    POMDOG_ASSERT(iter->value.IsObject());

    auto& attachmentObject = iter->value;

    POMDOG_ASSERT(!(!attachmentObject.HasMember("type")
        || !attachmentObject["type"].IsString()
        || std::strcmp(attachmentObject["type"].GetString(), "skinnedmesh") != 0));
    if (!attachmentObject.HasMember("type")
        || !attachmentObject["type"].IsString()
        || std::strcmp(attachmentObject["type"].GetString(), "skinnedmesh") != 0) {
        auto err = errors::New("invalid format");
        return std::make_tuple(SkinnedMeshAttachmentDesc{}, std::move(err));
    }

    POMDOG_ASSERT(!(!attachmentObject.HasMember("triangles") || !attachmentObject["triangles"].IsArray()));
    if (!attachmentObject.HasMember("triangles") || !attachmentObject["triangles"].IsArray()) {
        auto err = errors::New("invalid format");
        return std::make_tuple(SkinnedMeshAttachmentDesc{}, std::move(err));
    }

    POMDOG_ASSERT(!(!attachmentObject.HasMember("vertices") || !attachmentObject["vertices"].IsArray()));
    if (!attachmentObject.HasMember("vertices") || !attachmentObject["vertices"].IsArray()) {
        auto err = errors::New("invalid format");
        return std::make_tuple(SkinnedMeshAttachmentDesc{}, std::move(err));
    }

    POMDOG_ASSERT(!(!attachmentObject.HasMember("uvs") || !attachmentObject["uvs"].IsArray()));
    if (!attachmentObject.HasMember("uvs") || !attachmentObject["uvs"].IsArray()) {
        auto err = errors::New("invalid format");
        return std::make_tuple(SkinnedMeshAttachmentDesc{}, std::move(err));
    }

    SkinnedMeshAttachmentDesc desc;
    desc.Name = iter->name.GetString();
    desc.Vertices = ReadSkinnedMeshVertices(attachmentObject["vertices"], attachmentObject["uvs"]);
    desc.Indices = ReadSkinnedMeshIndices(attachmentObject["triangles"]);

    return std::make_tuple(std::move(desc), nullptr);
}

std::vector<SkinSlotDesc>
ReadSkinSlots(const rapidjson::Value& slotsDOM)
{
    POMDOG_ASSERT(slotsDOM.IsObject());

    std::vector<SkinSlotDesc> slots;

    for (auto slotIter = slotsDOM.MemberBegin(); slotIter != slotsDOM.MemberEnd(); ++slotIter) {
        if (!slotIter->name.IsString() || !slotIter->value.IsObject()) {
            ///@todo Not implemented
            // Error
            continue;
        }

        SkinSlotDesc slotDesc;
        slotDesc.SlotName = slotIter->name.GetString();

        auto& attachmentsDOM = slotIter->value;

        for (auto iter = attachmentsDOM.MemberBegin(); iter != attachmentsDOM.MemberEnd(); ++iter) {
            if (!iter->name.IsString() || !iter->value.IsObject()) {
                ///@todo Not implemented
                // Error
                continue;
            }

            auto& attachmentObject = iter->value;

            if (attachmentObject.HasMember("type")
                && attachmentObject["type"].IsString()
                && std::strcmp(attachmentObject["type"].GetString(), "skinnedmesh") == 0) {
                auto [desc, err] = ReadSkinnedMeshAttachment(iter);
                if (err != nullptr) {
                    ///@todo Not implemented
                    // Error
                    continue;
                }
                slotDesc.SkinnedMeshAttachments.push_back(std::move(desc));
            }
            else {
                slotDesc.Attachments.push_back(ReadAttachment(iter));
            }
        }

        slots.push_back(std::move(slotDesc));
    }

    return slots;
}

std::vector<SkinDesc> ReadSkins(const rapidjson::Value& skinsDOM)
{
    if (!skinsDOM.IsObject()) {
        ///@todo Not implemented
        // Error
        return {};
    }

    std::vector<SkinDesc> skins;

    for (auto iter = skinsDOM.MemberBegin(); iter != skinsDOM.MemberEnd(); ++iter) {
        if (!iter->name.IsString() || !iter->value.IsObject()) {
            ///@todo Not implemented
            // Error
            continue;
        }

        SkinDesc skinDesc;
        skinDesc.Name = iter->name.GetString();
        skinDesc.Slots = ReadSkinSlots(iter->value);
        skins.push_back(std::move(skinDesc));
    }

    return skins;
}

std::vector<AnimationSamplePointTranslate> ReadAnimationTranslateSamples(const rapidjson::Value& sampleDOM)
{
    if (!sampleDOM.IsArray()) {
        ///@todo Not implemented
        // Error
        return {};
    }

    std::vector<AnimationSamplePointTranslate> samplePoints;
    samplePoints.reserve(sampleDOM.Size());

    for (auto iter = sampleDOM.Begin(); iter != sampleDOM.End(); ++iter) {
        if (!iter->IsObject() || !iter->HasMember("time")) {
            // Error
            continue;
        }

        AnimationSamplePointTranslate samplePoint;
        samplePoint.Time = AnimationTimeInterval::zero();
        samplePoint.TranslateX = 0.0f;
        samplePoint.TranslateY = 0.0f;
        samplePoint.Curve = KeyframeCurve::Liener;

        ReadJsonMember(*iter, "time", samplePoint.Time);
        ReadJsonMember(*iter, "x", samplePoint.TranslateX);
        ReadJsonMember(*iter, "y", samplePoint.TranslateY);
        ReadJsonMember(*iter, "curve", samplePoint.Curve);

        samplePoints.push_back(std::move(samplePoint));
    }

    return samplePoints;
}

std::vector<AnimationSamplePointRotate> ReadAnimationRotateSamples(const rapidjson::Value& sampleDOM)
{
    if (!sampleDOM.IsArray()) {
        ///@todo Not implemented
        // Error
        return {};
    }

    std::vector<AnimationSamplePointRotate> samplePoints;
    samplePoints.reserve(sampleDOM.Size());

    for (auto iter = sampleDOM.Begin(); iter != sampleDOM.End(); ++iter) {
        if (!iter->IsObject() || !iter->HasMember("time")) {
            // Error
            continue;
        }

        AnimationSamplePointRotate samplePoint;
        samplePoint.Time = AnimationTimeInterval::zero();
        samplePoint.Curve = KeyframeCurve::Liener;

        ReadJsonMember(*iter, "time", samplePoint.Time);
        ReadJsonMember(*iter, "curve", samplePoint.Curve);

        Degree<float> degreeAngle = 0.0f;
        ReadJsonMember(*iter, "angle", degreeAngle);
        while (degreeAngle > 180.0f) {
            degreeAngle -= 360.0f;
        }
        while (degreeAngle < -180.0f) {
            degreeAngle += 360.0f;
        }
        samplePoint.Rotation = math::ToRadians(degreeAngle).value;

        samplePoints.push_back(std::move(samplePoint));
    }

    return samplePoints;
}

std::vector<AnimationSamplePointScale> ReadAnimationScaleSamples(const rapidjson::Value& sampleDOM)
{
    if (!sampleDOM.IsArray()) {
        ///@todo Not implemented
        // Error
        return {};
    }

    std::vector<AnimationSamplePointScale> samplePoints;
    samplePoints.reserve(sampleDOM.Size());

    for (auto iter = sampleDOM.Begin(); iter != sampleDOM.End(); ++iter) {
        if (!iter->IsObject() || !iter->HasMember("time")) {
            // Error
            continue;
        }

        AnimationSamplePointScale samplePoint;
        samplePoint.Time = AnimationTimeInterval::zero();
        samplePoint.Curve = KeyframeCurve::Liener;
        samplePoint.Scale = 1.0f;

        ReadJsonMember(*iter, "time", samplePoint.Time);
        ReadJsonMember(*iter, "curve", samplePoint.Curve);
        ReadJsonMember(*iter, "x", samplePoint.Scale);
        ReadJsonMember(*iter, "y", samplePoint.Scale);

        samplePoints.push_back(std::move(samplePoint));
    }

    return samplePoints;
}

std::vector<AnimationSamplePointAttachment> ReadAnimationAttachmentSamples(const rapidjson::Value& sampleDOM)
{
    if (!sampleDOM.IsArray()) {
        ///@todo Not implemented
        // Error
        return {};
    }

    std::vector<AnimationSamplePointAttachment> samplePoints;
    samplePoints.reserve(sampleDOM.Size());

    for (auto iter = sampleDOM.Begin(); iter != sampleDOM.End(); ++iter) {
        if (!iter->IsObject() || !iter->HasMember("time")) {
            // Error: Invalid file format
            continue;
        }

        if (!iter->HasMember("name")) {
            // Error: Invalid file format
            continue;
        }

        AnimationSamplePointAttachment samplePoint;
        samplePoint.Time = AnimationTimeInterval::zero();

        ReadJsonMember(*iter, "time", samplePoint.Time);
        ReadJsonMember(*iter, "name", samplePoint.AttachmentName);

        samplePoints.push_back(std::move(samplePoint));
    }

    return samplePoints;
}

std::vector<BoneAnimationTrackDesc> ReadBoneAnimationSamples(const rapidjson::Value& document)
{
    if (!document.IsObject()) {
        ///@todo Not implemented
        // Error
        return {};
    }

    std::vector<BoneAnimationTrackDesc> animationSamples;

    for (auto iter = document.MemberBegin(); iter != document.MemberEnd(); ++iter) {
        if (!iter->name.IsString() || !iter->value.IsObject()) {
            ///@todo Not implemented
            // Error
            continue;
        }

        BoneAnimationTrackDesc sample;
        sample.BoneName = iter->name.GetString();
        if (iter->value.HasMember("translate")) {
            sample.TranslateSamples = ReadAnimationTranslateSamples(iter->value["translate"]);
        }
        if (iter->value.HasMember("rotate")) {
            sample.RotateSamples = ReadAnimationRotateSamples(iter->value["rotate"]);
        }
        if (iter->value.HasMember("scale")) {
            sample.ScaleSamples = ReadAnimationScaleSamples(iter->value["scale"]);
        }
        animationSamples.push_back(std::move(sample));
    }

    return animationSamples;
}

std::vector<SlotAnimationTrackDesc> ReadSlotAnimationSamples(const rapidjson::Value& document)
{
    if (!document.IsObject()) {
        ///@todo Not implemented
        // Error: Invalid file format
        return {};
    }

    std::vector<SlotAnimationTrackDesc> animationSamples;

    for (auto iter = document.MemberBegin(); iter != document.MemberEnd(); ++iter) {
        if (!iter->name.IsString() || !iter->value.IsObject()) {
            ///@todo Not implemented
            // Error: Invalid file format
            continue;
        }

        SlotAnimationTrackDesc sample;
        sample.SlotName = iter->name.GetString();
        if (iter->value.HasMember("attachment")) {
            sample.AttachmentSamples = ReadAnimationAttachmentSamples(iter->value["attachment"]);
        }
        animationSamples.push_back(std::move(sample));
    }

    return animationSamples;
}

std::vector<AnimationClipDesc> ReadAnimationClips(const rapidjson::Value& document)
{
    if (!document.IsObject()) {
        ///@todo Not implemented
        // Error
        return {};
    }

    std::vector<AnimationClipDesc> animations;

    for (auto iter = document.MemberBegin(); iter != document.MemberEnd(); ++iter) {
        if (!iter->name.IsString() || !iter->value.IsObject()) {
            ///@todo Not implemented
            // Error
            continue;
        }

        AnimationClipDesc animationClip;
        animationClip.Name = iter->name.GetString();
        if (iter->value.HasMember("bones")) {
            animationClip.BoneTracks = ReadBoneAnimationSamples(iter->value["bones"]);
        }
        if (iter->value.HasMember("slots")) {
            animationClip.SlotTracks = ReadSlotAnimationSamples(iter->value["slots"]);
        }
        animations.push_back(std::move(animationClip));
    }

    return animations;
}

} // namespace

std::tuple<SkeletonDesc, std::unique_ptr<Error>>
SkeletonDescLoader::Load(const std::string& filePath)
{
    using detail::BinaryReader;

    std::ifstream stream{filePath, std::ifstream::binary};

    if (!stream) {
        auto err = errors::New("cannot open the file, " + filePath);
        return std::make_tuple(SkeletonDesc{}, std::move(err));
    }

    auto [byteLength, sizeErr] = FileSystem::GetFileSize(filePath);
    if (sizeErr != nullptr) {
        auto err = errors::Wrap(std::move(sizeErr), "failed to get file size, " + filePath);
        return std::make_tuple(SkeletonDesc{}, std::move(err));
    }

    POMDOG_ASSERT(stream);

    if (byteLength <= 0) {
        auto err = errors::New("the file is too small " + filePath);
        return std::make_tuple(SkeletonDesc{}, std::move(err));
    }

    auto json = BinaryReader::ReadArray<char>(stream, byteLength);
    POMDOG_ASSERT(!json.empty());

    json.push_back('\0');

    rapidjson::Document doc;
    doc.Parse(json.data());

    if (doc.HasParseError() || !doc.IsObject()) {
        auto err = errors::New("failed to parse JSON " + filePath);
        return std::make_tuple(SkeletonDesc{}, std::move(err));
    }

    spine::SkeletonDesc skeleton;

    if (doc.HasMember("bones")) {
        skeleton.Bones = ReadBones(doc["bones"]);
    }
    if (doc.HasMember("slots")) {
        skeleton.Slots = ReadSlots(doc["slots"], skeleton.Bones);
    }
    if (doc.HasMember("skins")) {
        skeleton.Skins = ReadSkins(doc["skins"]);
    }
    if (doc.HasMember("animations")) {
        skeleton.AnimationClips = ReadAnimationClips(doc["animations"]);
    }

    return std::make_tuple(std::move(skeleton), nullptr);
}

} // namespace pomdog::spine
