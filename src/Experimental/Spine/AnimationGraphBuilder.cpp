// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#include "Pomdog/Experimental/Spine/AnimationGraphBuilder.hpp"
#include "Pomdog/Content/Utility/BinaryReader.hpp"
#include "Pomdog/Experimental/Skeletal2D/AnimationClip.hpp"
#include "Pomdog/Experimental/Skeletal2D/BlendTrees/AnimationClipNode.hpp"
#include "Pomdog/Experimental/Skeletal2D/BlendTrees/AnimationLerpNode.hpp"
#include "Pomdog/Experimental/Spine/AnimationLoader.hpp"
#include "Pomdog/Experimental/Spine/SkeletonDesc.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Utility/FileSystem.hpp"
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wzero-as-null-pointer-constant"
#endif
#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable : 5054)
#endif
#include <rapidjson/document.h>
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
#if defined(_MSC_VER)
#pragma warning(pop)
#endif
#include <fstream>
#include <optional>
#include <vector>

namespace Pomdog::Spine {
namespace {

using Detail::BinaryReader;
using Skeletal2D::AnimationBlendInput;
using Skeletal2D::AnimationBlendInputType;
using Skeletal2D::AnimationClip;
using Skeletal2D::AnimationGraphState;
using Skeletal2D::AnimationNode;
using Skeletal2D::Detail::AnimationClipNode;
using Skeletal2D::Detail::AnimationLerpNode;

enum class AnimationNodeType : std::uint8_t {
    Clip,
    Lerp,
};

struct AnimationNodeDesc final {
    std::optional<std::string> ClipName;
    std::optional<std::string> Parameter;
    std::array<std::optional<std::string>, 4> Inputs;
    std::optional<float> DefaultParameter;
    std::string Name;
    AnimationNodeType Type;
};

std::tuple<std::unique_ptr<AnimationNode>, std::shared_ptr<Error>>
CreateAnimationNode(
    const AnimationNodeDesc& desc,
    std::vector<AnimationBlendInput>& inputs,
    const std::vector<AnimationNodeDesc>& nodes,
    const SkeletonDesc& skeletonDesc)
{
    switch (desc.Type) {
    case AnimationNodeType::Clip: {
        if (desc.ClipName == std::nullopt) {
            auto err = Errors::New("ClipName is nullopt");
            return std::make_tuple(nullptr, std::move(err));
        }
        auto [animationClip, err] = Spine::CreateAnimationClip(skeletonDesc, std::nullopt, *desc.ClipName);
        if (err != nullptr) {
            auto wrapped = Errors::Wrap(std::move(err), "failed to create animation clip, " + *desc.ClipName);
            return std::make_tuple(nullptr, std::move(wrapped));
        }
        auto node = std::make_unique<AnimationClipNode>(animationClip);
        return std::make_tuple(std::move(node), nullptr);
        break;
    }
    case AnimationNodeType::Lerp: {
        POMDOG_ASSERT(desc.Inputs[0]);
        POMDOG_ASSERT(desc.Inputs[1]);

        auto iter1 = std::find_if(std::begin(nodes), std::end(nodes),
            [&](const AnimationNodeDesc& nodeDesc) { return nodeDesc.Name == *desc.Inputs[0]; });
        POMDOG_ASSERT(iter1 != std::end(nodes));

        auto iter2 = std::find_if(std::begin(nodes), std::end(nodes),
            [&](const AnimationNodeDesc& nodeDesc) { return nodeDesc.Name == *desc.Inputs[1]; });
        POMDOG_ASSERT(iter2 != std::end(nodes));

        auto [node1, err1] = CreateAnimationNode(*iter1, inputs, nodes, skeletonDesc);
        if (err1 != nullptr) {
            auto wrapped = Errors::Wrap(std::move(err1), "failed to create lerp input node[0]");
            return std::make_tuple(nullptr, std::move(wrapped));
        }

        auto [node2, err2] = CreateAnimationNode(*iter2, inputs, nodes, skeletonDesc);
        if (err2 != nullptr) {
            auto wrapped = Errors::Wrap(std::move(err2), "failed to create lerp input node[1]");
            return std::make_tuple(nullptr, std::move(wrapped));
        }

        POMDOG_ASSERT(node1 != nullptr);
        POMDOG_ASSERT(node2 != nullptr);

        if (desc.Parameter) {
            inputs.push_back({*desc.Parameter, AnimationBlendInputType::Float});
        }
        else {
            inputs.push_back({"__unknown", AnimationBlendInputType::Float});
        }
        const auto parameterIndex = static_cast<std::uint16_t>(inputs.size() - 1);

        auto node = std::make_unique<AnimationLerpNode>(std::move(node1), std::move(node2), parameterIndex);
        return std::make_tuple(std::move(node), nullptr);
        break;
    }
    }

    auto err = Errors::New("unknown animation node type");
    return std::make_tuple(nullptr, std::move(err));
}

} // namespace

std::tuple<std::shared_ptr<Skeletal2D::AnimationGraph>, std::shared_ptr<Error>>
LoadAnimationGraph(const SkeletonDesc& skeletonDesc, const std::string& filePath)
{
    std::ifstream stream{filePath, std::ifstream::binary};

    if (!stream) {
        auto err = Errors::New("cannot open the file, " + filePath);
        return std::make_tuple(nullptr, std::move(err));
    }

    auto [byteLength, sizeErr] = FileSystem::GetFileSize(filePath);
    if (sizeErr != nullptr) {
        auto err = Errors::Wrap(std::move(sizeErr), "failed to get file size, " + filePath);
        return std::make_tuple(nullptr, std::move(err));
    }

    POMDOG_ASSERT(stream);

    if (byteLength <= 0) {
        auto err = Errors::New("the file is too small " + filePath);
        return std::make_tuple(nullptr, std::move(err));
    }

    auto json = BinaryReader::ReadArray<char>(stream, byteLength);
    POMDOG_ASSERT(!json.empty());

    json.push_back('\0');

    rapidjson::Document doc;
    doc.Parse(json.data());

    if (doc.HasParseError()) {
        auto err = Errors::New("failed to parse JSON, " + filePath);
        return std::make_tuple(nullptr, std::move(err));
    }

    if (!doc.IsObject()) {
        auto err = Errors::New("invalid file format " + filePath);
        return std::make_tuple(nullptr, std::move(err));
    }

    std::vector<AnimationNodeDesc> nodes;
    if (doc.HasMember("nodes") && doc["nodes"].IsArray()) {
        auto& nodeArray = doc["nodes"];
        for (auto iter = nodeArray.Begin(); iter != nodeArray.End(); ++iter) {
            POMDOG_ASSERT(iter->IsObject());

            auto& nodeObject = *iter;
            AnimationNodeDesc desc;

            {
                POMDOG_ASSERT(iter->HasMember("type"));
                auto& typeObject = nodeObject["type"];

                POMDOG_ASSERT(typeObject.IsString());
                if (0 == std::strncmp(typeObject.GetString(), "clip", 3)) {
                    desc.Type = AnimationNodeType::Clip;
                }
                else if (0 == std::strncmp(typeObject.GetString(), "lerp", 3)) {
                    desc.Type = AnimationNodeType::Lerp;
                }
                else {
                    auto err = Errors::New("invalid type, " + std::string{typeObject.GetString()});
                    return std::make_tuple(nullptr, std::move(err));
                }
            }
            {
                POMDOG_ASSERT(iter->HasMember("name"));
                auto& idObject = nodeObject["name"];

                POMDOG_ASSERT(idObject.IsString());
                desc.Name = idObject.GetString();
            }

            if (desc.Type == AnimationNodeType::Clip && nodeObject.HasMember("name") && nodeObject["name"].IsString()) {
                desc.ClipName = std::make_optional(nodeObject["name"].GetString());
            }
            if (nodeObject.HasMember("param_name") && nodeObject["param_name"].IsString()) {
                desc.Parameter = std::make_optional(nodeObject["param_name"].GetString());
            }
            if (nodeObject.HasMember("in") && nodeObject["in"].IsArray()) {
                auto& inputs = nodeObject["in"];
                auto count = std::min(static_cast<int>(inputs.Size()), static_cast<int>(desc.Inputs.size()));
                for (int i = 0; i < count; ++i) {
                    POMDOG_ASSERT(inputs[i].IsString());
                    desc.Inputs[i] = std::make_optional(inputs[i].GetString());
                }
            }
            nodes.push_back(std::move(desc));
        }
    }

    std::vector<AnimationBlendInput> inputs;
    auto animationGraph = std::make_shared<Skeletal2D::AnimationGraph>();

    if (doc.HasMember("states") && doc["states"].IsObject()) {
        auto& stateArray = doc["states"];
        for (auto iter = stateArray.MemberBegin(); iter != stateArray.MemberEnd(); ++iter) {
            AnimationGraphState state;

            POMDOG_ASSERT(iter->name.IsString());
            state.Name = iter->name.GetString();

            POMDOG_ASSERT(iter->value["tree"].IsString());
            const auto rootNodeName = iter->value["tree"].GetString();

            auto rootNodeDesc = std::find_if(std::begin(nodes), std::end(nodes),
                [&rootNodeName](const AnimationNodeDesc& desc) { return desc.Name == rootNodeName; });
            POMDOG_ASSERT(rootNodeDesc != std::end(nodes));

            auto [node, err] = CreateAnimationNode(*rootNodeDesc, inputs, nodes, skeletonDesc);
            if (err != nullptr) {
                auto wrapped = Errors::Wrap(std::move(err), "failed to create animation node");
                return std::make_tuple(nullptr, std::move(wrapped));
            }

            state.Tree = std::move(node);
            animationGraph->States.push_back(std::move(state));
        }
    }

    animationGraph->Inputs = std::move(inputs);

    return std::make_tuple(std::move(animationGraph), nullptr);
}

} // namespace Pomdog::Spine
