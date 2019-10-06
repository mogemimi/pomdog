// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#include "AnimationGraphBuilder.hpp"
#include "Pomdog.Experimental/Spine/AnimationLoader.hpp"
#include "Pomdog.Experimental/Spine/SkeletonDesc.hpp"
#include "Pomdog.Experimental/Skeletal2D/detail/AnimationClipNode.hpp"
#include "Pomdog.Experimental/Skeletal2D/detail/AnimationLerpNode.hpp"
#include "Pomdog.Experimental/Skeletal2D/AnimationClip.hpp"
#include "Pomdog/Content/Utility/BinaryReader.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Utility/FileSystem.hpp"
#include <rapidjson/document.h>
#include <fstream>
#include <optional>
#include <vector>

namespace Pomdog::Spine {
namespace {

using Detail::BinaryReader;

enum class AnimationNodeType: std::uint8_t {
    Clip,
    Lerp,
};

struct AnimationNodeDesc {
    std::optional<std::string> ClipName;
    std::optional<std::string> Parameter;
    std::array<std::optional<std::string>, 4> Inputs;
    std::optional<float> DefaultParameter;
    std::string Name;
    AnimationNodeType Type;
};

std::unique_ptr<AnimationNode> CreateAnimationNode(
    AnimationNodeDesc const& desc, std::vector<AnimationBlendInput> & inputs,
        std::vector<AnimationNodeDesc> const& nodes, SkeletonDesc const& skeletonDesc)
{
    using Detail::Skeletal2D::AnimationClipNode;
    using Detail::Skeletal2D::AnimationLerpNode;

    switch (desc.Type) {
    case AnimationNodeType::Clip: {
        POMDOG_ASSERT(desc.ClipName);
        auto animationClip = std::make_shared<AnimationClip>(Spine::CreateAnimationClip(skeletonDesc, desc.ClipName->c_str()));
        return std::make_unique<AnimationClipNode>(animationClip);
        break;
    }
    case AnimationNodeType::Lerp: {
        POMDOG_ASSERT(desc.Inputs[0]);
        POMDOG_ASSERT(desc.Inputs[1]);

        auto iter1 = std::find_if(std::begin(nodes),std::end(nodes),
            [&](AnimationNodeDesc const& nodeDesc){ return nodeDesc.Name == *desc.Inputs[0]; });
        POMDOG_ASSERT(iter1 != std::end(nodes));

        auto iter2 = std::find_if(std::begin(nodes),std::end(nodes),
            [&](AnimationNodeDesc const& nodeDesc){ return nodeDesc.Name == *desc.Inputs[1]; });
        POMDOG_ASSERT(iter2 != std::end(nodes));

        auto node1 = CreateAnimationNode(*iter1, inputs, nodes, skeletonDesc);
        auto node2 = CreateAnimationNode(*iter2, inputs, nodes, skeletonDesc);
        POMDOG_ASSERT(node1 != nullptr);
        POMDOG_ASSERT(node2 != nullptr);

        if (desc.Parameter) {
            inputs.push_back({*desc.Parameter, AnimationBlendInputType::Float});
        }
        else {
            inputs.push_back({"__unknown", AnimationBlendInputType::Float});
        }
        std::uint16_t parameterIndex = inputs.size() - 1;

        return std::make_unique<AnimationLerpNode>(std::move(node1), std::move(node2), parameterIndex);
        break;
    }
    }
    return nullptr;
}

} // unnamed namespace

std::tuple<std::shared_ptr<AnimationGraph>, std::shared_ptr<Error>>
LoadAnimationGraph(const SkeletonDesc& skeletonDesc, const std::string& filePath)
{
    std::ifstream stream{filePath, std::ifstream::binary};

    if (!stream) {
        auto err = Errors::New("cannot open the file, " + filePath);
        return std::make_tuple(nullptr, std::move(err));
    }

    auto[byteLength, sizeErr] = FileSystem::GetFileSize(filePath);
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
    if (doc.HasMember("nodes") && doc["nodes"].IsArray())
    {
        auto & nodeArray = doc["nodes"];
        for (auto iter = nodeArray.Begin(); iter != nodeArray.End(); ++iter)
        {
            POMDOG_ASSERT(iter->IsObject());

            auto & nodeObject = *iter;
            AnimationNodeDesc desc;

            {
                POMDOG_ASSERT(iter->HasMember("type"));
                auto & typeObject = nodeObject["type"];

                POMDOG_ASSERT(typeObject.IsString());
                if (0 == std::strncmp(typeObject.GetString(), "clip", 3)) {
                    desc.Type = AnimationNodeType::Clip;
                }
                else if (0 == std::strncmp(typeObject.GetString(), "lerp", 3)) {
                    desc.Type = AnimationNodeType::Lerp;
                }
                else {
                    ///@todo Error: Not implemented
                    continue;
                }
            }
            {
                POMDOG_ASSERT(iter->HasMember("name"));
                auto & idObject = nodeObject["name"];

                POMDOG_ASSERT(idObject.IsString());
                desc.Name = idObject.GetString();
            }

            if (desc.Type == AnimationNodeType::Clip
                && nodeObject.HasMember("name") && nodeObject["name"].IsString()) {
                desc.ClipName = std::make_optional(nodeObject["name"].GetString());
            }
            if (nodeObject.HasMember("param_name") && nodeObject["param_name"].IsString()) {
                desc.Parameter = std::make_optional(nodeObject["param_name"].GetString());
            }
            if (nodeObject.HasMember("in") && nodeObject["in"].IsArray())
            {
                auto & inputs = nodeObject["in"];
                auto count = std::min<int>(inputs.Size(), desc.Inputs.size());
                for (int i = 0; i < count; ++i)
                {
                    POMDOG_ASSERT(inputs[i].IsString());
                    desc.Inputs[i] = std::make_optional(inputs[i].GetString());
                }
            }
            nodes.push_back(std::move(desc));
        }
    }

    std::vector<AnimationBlendInput> inputs;
    auto animationGraph = std::make_shared<AnimationGraph>();

    if (doc.HasMember("states") && doc["states"].IsObject())
    {
        auto & stateArray = doc["states"];
        for (auto iter = stateArray.MemberBegin(); iter != stateArray.MemberEnd(); ++iter)
        {
            AnimationGraphState state;

            POMDOG_ASSERT(iter->name.IsString());
            state.Name = iter->name.GetString();

            POMDOG_ASSERT(iter->value["tree"].IsString());
            const auto rootNodeName = iter->value["tree"].GetString();

            auto rootNodeDesc = std::find_if(std::begin(nodes),std::end(nodes),
                [&rootNodeName](AnimationNodeDesc const& desc){ return desc.Name == rootNodeName; });
            POMDOG_ASSERT(rootNodeDesc != std::end(nodes));

            state.Tree = CreateAnimationNode(*rootNodeDesc, inputs, nodes, skeletonDesc);
            animationGraph->States.push_back(std::move(state));
        }
    }

    animationGraph->Inputs = std::move(inputs);

    return std::make_tuple(std::move(animationGraph), nullptr);
}

} // namespace Pomdog::Spine
